#include "AssetConverterSkeleton.hpp"

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Engine/Geometry/MathFunctions.hpp>
#include <Utility/Log.hpp>

AssetConverterSkeleton::AssetConverterSkeleton() {
}

AssetConverterSkeleton::~AssetConverterSkeleton() {
}

bool AssetConverterSkeleton::Convert(const char* filepath, const char* destination, bool isSkeleton) {
    success = true;
    errorString = "";

    const aiScene* aScene = aImporter.ReadFile(filepath, 0);

    bones.clear();
    bones.shrink_to_fit();

    if (!aScene->HasAnimations()) {
        errorString.append("Error: No animation in scene.");
        success = false;

        // Free memory.
        aImporter.FreeScene();
        return false;
    }

    // Push back the bones.
    for (unsigned int i = 0; i < aScene->mMeshes[0]->mNumBones; ++i)
        if (aScene->mMeshes[0]->mBones[i]->mName.C_Str() != aScene->mMeshes[0]->mName.C_Str())
            bones.push_back(aScene->mMeshes[0]->mBones[i]->mName.C_Str());

    currentId = -1;
    SceneRecursive(aScene->mRootNode, -1);

    if (isSkeleton) {
        Animation::Skeleton skeleton;
        for (unsigned int i = 0; i < bones.size(); ++i) {
            auto boneIndex = 0;
            for (unsigned int j = 0; j < aScene->mAnimations[0]->mNumChannels; ++j)
                if (aScene->mAnimations[0]->mChannels[j]->mNodeName.C_Str() == bones[i]) {
                    boneIndex = j;
                    break;
                }

            aiNodeAnim* channel = aScene->mAnimations[0]->mChannels[boneIndex];
            Animation::SkeletonBone* bone = new Animation::SkeletonBone;
            bone->parentId = (uint32_t)parents[i];

            // Build bindpose.
            glm::mat4 scaleMatrix(1.f);
            glm::mat4 posMatrix(1.f);

            glm::quat rot;
            rot.x = channel->mRotationKeys[0].mValue.x;
            rot.y = channel->mRotationKeys[0].mValue.y;
            rot.z = channel->mRotationKeys[0].mValue.z;
            rot.w = channel->mRotationKeys[0].mValue.w;

            glm::mat4 matRot(rot);

            glm::vec3 pos;
            pos.x = channel->mPositionKeys[0].mValue.x;
            pos.y = channel->mPositionKeys[0].mValue.y;
            pos.z = channel->mPositionKeys[0].mValue.z;
            posMatrix = glm::translate(posMatrix, pos);

            glm::vec3 scale(1.f, 1.f, 1.f);
            scaleMatrix = glm::scale(scaleMatrix, scale);

            bone->localTx = posMatrix * (matRot * scaleMatrix);

            skeleton.skeletonBones.push_back(bone);
        }

        // Save to file.
        skeleton.Save((std::string(destination)).c_str());
    } else {
        Animation::Animation anim;
        anim.numBones = static_cast<uint32_t>(bones.size());
        anim.bones = new Animation::Bone[aScene->mAnimations[0]->mNumChannels];
        anim.length = 0;

        // Positions for root positions.
        auto rootIndex = 0;
        for (unsigned int index = 0; index < aScene->mAnimations[0]->mNumChannels; ++index)
            if (aScene->mAnimations[0]->mChannels[index]->mNodeName.C_Str() == bones[0]) {
                rootIndex = index;
                break;
            }

        anim.numRootPositions = aScene->mAnimations[0]->mChannels[rootIndex]->mNumPositionKeys;
        anim.rootPositionKeys = new int32_t[anim.numRootPositions];
        anim.rootPositions = new glm::vec3[anim.numRootPositions];

        for (unsigned int i = 0; i < anim.numRootPositions; ++i) {
            anim.rootPositionKeys[i] = (int32_t)aScene->mAnimations[0]->mChannels[rootIndex]->mPositionKeys[i].mTime;
            aiVector3D position = aScene->mAnimations[0]->mChannels[rootIndex]->mPositionKeys[i].mValue;
            anim.rootPositions[i] = glm::vec3(position.x, position.y, position.z);
        }

        for (unsigned int i = 0; i < bones.size(); ++i) {
            auto boneIndex = 0;
            for (unsigned int j = 0; j < aScene->mAnimations[0]->mNumChannels; ++j)
                if (aScene->mAnimations[0]->mChannels[j]->mNodeName.C_Str() == bones[i]) {
                    boneIndex = j;
                    break;
                }

            aiNodeAnim* channel = aScene->mAnimations[0]->mChannels[boneIndex];
            anim.bones[i].parent = (uint32_t)parents[i];
            anim.bones[i].numRotationKeys = channel->mNumRotationKeys;
            anim.bones[i].rotationKeys = new int32_t[channel->mNumRotationKeys];
            anim.bones[i].rotations = new glm::quat[channel->mNumRotationKeys];

            // Build keyframes for the bone.
            for (unsigned int j = 0; j < channel->mNumRotationKeys; ++j) {
                anim.bones[i].rotationKeys[j] = (int32_t)channel->mRotationKeys[j].mTime;

                if (anim.bones[i].rotationKeys[j] > anim.length)
                    anim.length = anim.bones[i].rotationKeys[j];

                glm::quat rot;
                rot.x = channel->mRotationKeys[j].mValue.x;
                rot.y = channel->mRotationKeys[j].mValue.y;
                rot.z = channel->mRotationKeys[j].mValue.z;
                rot.w = channel->mRotationKeys[j].mValue.w;

                anim.bones[i].rotations[j] = rot;
            }
        }

        // Save.
        std::ofstream file((std::string(destination)).c_str(), std::ios::binary);
        anim.Save(&file);
        file.close();
    }

    // Free memory.
    aImporter.FreeScene();

    return true;
}

bool AssetConverterSkeleton::Success() const {
    return success;
}

const std::string& AssetConverterSkeleton::GetErrorString() {
    return errorString;
}

void AssetConverterSkeleton::SceneRecursive(aiNode* node, int parent) {
    if (!node)
        return;

    int thisId = currentId;

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        aiNode* child = node->mChildren[i];
        bool foundBone = false;
        for (std::size_t j = 0; j < bones.size(); ++j)
            if (child->mName.C_Str() == bones[j]) {
                children.push_back(child->mName.C_Str());
                ids.push_back(0);
                ++currentId;
                parents.push_back(-1);
                BoneRecursive(child, 0);
                foundBone = true;
            }

        if (!foundBone)
            SceneRecursive(child, thisId);
    }
}

void AssetConverterSkeleton::BoneRecursive(aiNode* node, int parent) {
    if (!node)
        return;

    int thisId = currentId;

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        aiNode* child = node->mChildren[i];
        bool foundBone = false;
        for (std::size_t j = 0; j < bones.size() && !foundBone; ++j)
            if (child->mName.C_Str() == bones[j]) {
                children.push_back(child->mName.C_Str());
                ids.push_back(thisId);

                for (std::size_t k = 0; k < bones.size(); ++k)
                    if (bones[k] == child->mParent->mName.C_Str()) {
                        parents.push_back(static_cast<int>(k));
                        break;
                    }

                ++currentId;
                foundBone = true;
                BoneRecursive(child, thisId);
            }

        if (!foundBone)
            return;
    }
}
