#include "Skeleton.hpp"
#include "Animation.hpp"
#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>
#include "MathFunctions.hpp"

using namespace Geometry;

Skeleton::Skeleton() {

}

Skeleton::Skeleton(const aiScene* aScene) {
    Load(aScene);
}

Skeleton::~Skeleton() {

}

void Skeleton::Load(const aiScene* aScene) {
    // Calculate global inverse transform.
    CpyMat(globalInverseTransform, aScene->mRootNode->mTransformation);
    globalInverseTransform = glm::inverse(globalInverseTransform);

    // Load node tree.
    LoadNodeTree(aScene->mRootNode, &rootNode, nullptr);

    // Count bones.
    std::size_t numBones = 0;
    for (unsigned int m = 0; m < aScene->mNumMeshes; ++m)
        numBones += aScene->mMeshes[m]->mNumBones;
    bones.resize(numBones);

    // Load bones.
    std::size_t countBones = 0;
    for (unsigned int m = 0; m < aScene->mNumMeshes; ++m) {
        const aiMesh* aMesh = aScene->mMeshes[m];
        for (unsigned int b = 0; b < aMesh->mNumBones; ++b) {
            const aiBone* aBone = aMesh->mBones[b];
            const std::string boneName = aBone->mName.data;
            const glm::mat4* bone = FindBone(boneName);
            if (bone == nullptr) {
                // Found new bone.
                CpyMat(bones[countBones], aBone->mOffsetMatrix);
                boneIndexMap[boneName] = countBones++;
            }
        }
    }
    bones.resize(countBones);
    finalTransforms.resize(countBones);
}

std::size_t Skeleton::GetNumBones() const {
    return bones.size();
}

void Skeleton::Animate(const Animation* animation, const float timeInSeconds) {
    float animationTime = 0;
    float ticksPerSecond = (float)(animation->ticksPerSecond != 0 ? animation->ticksPerSecond : 25.0f);
    float timeInTicks = timeInSeconds * ticksPerSecond;
    animationTime = fmod(timeInTicks, static_cast<float>(animation->duration));

    ReadNodeHeirarchy(animation, timeInSeconds, &rootNode, glm::mat4());
}

void Skeleton::BindPose() {
    ReadNodeHeirarchy(nullptr, 0, &rootNode, glm::mat4());
}

void Skeleton::LoadNodeTree(aiNode* aNode, Node* node, Node* parentNode) {
    node->name = aNode->mName.C_Str();
    CpyMat(node->transformation, aNode->mTransformation);
    node->parent = parentNode;
    node->children.resize(aNode->mNumChildren);
    for (std::size_t i = 0; i < aNode->mNumChildren; ++i) {
        LoadNodeTree(aNode->mChildren[i], &node->children[i], node);
    }
}

void Skeleton::ReadNodeHeirarchy(const Animation* animation, float animationTime, Node* node, const glm::mat4& parentTransform) {
    glm::mat4 nodeTransformation(node->transformation);

    animationTime = 50;

    if (animation != nullptr) {
        const Animation::AnimChannel* channel = animation->FindChannel(node->name);
        if (channel != nullptr) {
            // Interpolate scaling and generate scaling transformation matrix.
            glm::vec3 scaling;
            Animation::CalcInterpolatedScaling(scaling, animationTime, channel);
            glm::mat4 scalingM(glm::scale(glm::mat4(), scaling));

            // Interpolate rotation and generate rotation transformation matrix.
            aiQuaternion rotationQ;
            Animation::CalcInterpolatedRotation(rotationQ, animationTime, channel);
            glm::mat4 rotationM;
            aiMatrix3x3 aMat = rotationQ.GetMatrix();
            CpyMat(rotationM, aiMatrix4x4(aMat));

            // Interpolate translation and generate translation transformation matrix.
            glm::vec3 translation;
            Animation::CalcInterpolatedPosition(translation, animationTime, channel);
            glm::mat4 translationM(glm::translate(glm::mat4(), translation));

            //// Combine the above transformations.
            nodeTransformation = scalingM * rotationM * glm::transpose(translationM);
        }
    }

    glm::mat4 globalTransformation = nodeTransformation * parentTransform;

    const auto& it = boneIndexMap.find(node->name);
    if (it != this->boneIndexMap.end()) {
        size_t boneIndex = it->second;
        finalTransforms[boneIndex] = glm::transpose(bones[boneIndex] * globalTransformation * this->globalInverseTransform); // TODO TRANSPOSE?
    }

    for (std::size_t i = 0; i < node->children.size(); ++i) {
        ReadNodeHeirarchy(animation, animationTime, &node->children[i], globalTransformation);
    }
}

const glm::mat4* Skeleton::FindBone(const std::string& name) const {
    const auto& it = this->boneIndexMap.find(name);
    if (it != boneIndexMap.end())
        return &bones[it->second];
    return nullptr;
}

std::size_t Skeleton::FindBoneIndex(const std::string& name) {
    const auto& it = this->boneIndexMap.find(name);
    if (it != boneIndexMap.end())
        return it->second;
    return -1;
}

const std::vector<glm::mat4>& Skeleton::GetFinalTransformations() const {
    return finalTransforms;
}
