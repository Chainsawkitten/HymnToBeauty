#include "RiggedModel.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include "../Util/Log.hpp"
#include "../Util/Log.hpp"

using namespace Geometry;

RiggedModel::RiggedModel() {

}

RiggedModel::RiggedModel(const char* filename) {
    Load(filename);
}

RiggedModel::~RiggedModel() {
}

void RiggedModel::Load(const char* filename) {
    // Load geometry data.
    const aiScene* aScene = aImporter.ReadFile(filename,
        aiProcess_CalcTangentSpace | \
        aiProcess_GenSmoothNormals | \
        aiProcess_JoinIdenticalVertices | \
        aiProcess_ImproveCacheLocality | \
        aiProcess_LimitBoneWeights | \
        aiProcess_RemoveRedundantMaterials | \
        aiProcess_SplitLargeMeshes | \
        aiProcess_Triangulate | \
        aiProcess_GenUVCoords | \
        aiProcess_SortByPType | \
        aiProcess_FindDegenerates | \
        /*aiProcess_FindInvalidData | \*/
        aiProcess_ValidateDataStructure | \
        0);

    Log() << aImporter.GetErrorString() << "\n";

    assert(aScene != nullptr);

    // Calculate global inverse transform.
    CpyMat(globalInverseTransform, aScene->mRootNode->mTransformation);
    globalInverseTransform = glm::inverse(globalInverseTransform);

    // Load node tree.
    LoadNodeTree(aScene->mRootNode, &rootNode, nullptr);
        
    // Load meshes.
    LoadMeshes(aScene);

    // Load animation data.
    LoadAnimations(aScene);

    // TMP
    if (!animations.empty())
        BoneTransform(0.0f, animations[0]);

    std::vector<glm::mat4> transforms;
    transforms.resize(this->numBones);
    for (unsigned int i = 0; i < this->numBones; i++) {
        transforms[i] = this->boneInfos[i].finalTransformation;
    }
    // ~TMP


    // Generate buffers.
    GenerateVertexBuffer(vertexBuffer);
    GenerateIndexBuffer(indices.data(), indices.size(), indexBuffer);
    GenerateVertexArray(vertexBuffer, indexBuffer, vertexArray);

    // Generate AABB
    CreateAxisAlignedBoundingBox(verticesPos);

    // Clear vectors.
    vertices.clear();
    vertices.shrink_to_fit();
    verticesPos.clear();
    verticesPos.shrink_to_fit();
    indices.clear();
    indices.shrink_to_fit();
}

const Geometry3D::Type RiggedModel::GetType() const {
    return SKIN;
}

void RiggedModel::GenerateVertexBuffer(GLuint& vertexBuffer) {
    vertexBuffer = VertexType::SkinVertex::GenerateVertexBuffer(vertices.data(), vertices.size());
}

void RiggedModel::GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray) {
    vertexArray = VertexType::SkinVertex::GenerateVertexArray(vertexBuffer, indexBuffer);
}

void RiggedModel::LoadMeshes(const aiScene* aScene) {
    std::vector<MeshEntry> entries;
    entries.resize(aScene->mNumMeshes);

    std::size_t numVertices = 0;
    std::size_t numIndices = 0;

    // Count the number of vertices and indices.
    for (unsigned int i = 0; i < aScene->mNumMeshes; ++i) {
        entries[i].numIndices = aScene->mMeshes[i]->mNumFaces * 3;
        entries[i].baseVertex = numVertices;
        entries[i].baseIndex = numIndices;

        numVertices += aScene->mMeshes[i]->mNumVertices;
        numIndices += entries[i].numIndices;
    }

    // Resize vectors to fit.
    vertices.resize(numVertices);
    verticesPos.resize(numVertices);
    indices.resize(numIndices);
    boneVertDatas.resize(numVertices);

    numVertices = 0;
    numIndices = 0;

    // Initialize the meshes in the scene one by one.
    for (unsigned int m = 0; m < aScene->mNumMeshes; ++m) {
        const aiMesh* aMesh = aScene->mMeshes[m];
        // Load vertices.
        for (unsigned int i = 0; i < aMesh->mNumVertices; ++i) {
            VertexType::SkinVertex& vert = vertices[numVertices];
            CpyVec(vert.position, aMesh->mVertices[i]);
            CpyVec(vert.textureCoordinate, aMesh->mTextureCoords[0][i]);
            CpyVec(vert.normal, aMesh->mNormals[i]);
            CpyVec(vert.tangent, aMesh->mTangents[i]);
            verticesPos[numVertices] = &vertices[numVertices].position;
            numVertices++;
        }
        // Load indices.
        for (unsigned int i = 0; i < aMesh->mNumFaces; ++i) {
            const aiFace& aFace = aMesh->mFaces[i];
            assert(aFace.mNumIndices == 3);
            indices[numIndices++] = entries[m].baseVertex + aFace.mIndices[0];
            indices[numIndices++] = entries[m].baseVertex + aFace.mIndices[1];
            indices[numIndices++] = entries[m].baseVertex + aFace.mIndices[2];
        }
        // Load bones.
        for (unsigned int b = 0; b < aMesh->mNumBones; ++b) {
            const aiBone* aBone = aMesh->mBones[b];
            unsigned int boneIndex = 0;
            std::string boneName(aBone->mName.data);

            // Check in bone map if it's a new bone.
            const auto& it = this->boneMap.find(boneName);
            if (it == this->boneMap.end()) {
                // Allocate an index for a new bone.
                boneIndex = this->numBones;
                this->numBones++;
                BoneInfo boneInfo;
                CpyMat(boneInfo.boneOffset, aBone->mOffsetMatrix);
                this->boneInfos.push_back(boneInfo);
                this->boneMap[boneName] = boneIndex;
            } else {
                // Get an index for an old bone.
                boneIndex = it->second;
            }
            // Load Weights.
            for (unsigned int i = 0; i < aBone->mNumWeights; ++i) {
                unsigned int vertexID = entries[m].baseVertex + aBone->mWeights[i].mVertexId;
                float weight = aBone->mWeights[i].mWeight;
                boneVertDatas[vertexID].AddBoneData(boneIndex, weight);
            }
        }
    }
}

void RiggedModel::LoadNodeTree(aiNode* aNode, Node* node, Node* parentNode) {
    node->name = aNode->mName.C_Str();
    CpyMat(node->transformation, aNode->mTransformation);
    node->parent = parentNode;
    node->children.resize(aNode->mNumChildren);
    for (std::size_t i = 0; i < aNode->mNumChildren; ++i) {
        LoadNodeTree(aNode->mChildren[i], &node->children[i], node);
    }
}

void RiggedModel::LoadAnimations(const aiScene* aScene) {
    animations.resize(aScene->mNumAnimations);

    // Initialize the animations in the scene one by one.
    for (unsigned int a = 0; a < aScene->mNumAnimations; ++a) {
        const aiAnimation* aAnimation = aScene->mAnimations[a];
        Animation& animation = animations[a];
        animation.name = aAnimation->mName.data;
        animation.duration = aAnimation->mDuration;
        animation.ticksPerSecond = aAnimation->mTicksPerSecond;
        animation.animChanelNr = aAnimation->mNumChannels;
        animation.animChanelData = new Animation::AnimChanel[animation.animChanelNr];
        for (unsigned int c = 0; c < animation.animChanelNr; ++c) {
            Animation::AnimChanel* animChanel = &animation.animChanelData[c];
            const aiNodeAnim* aAnimChanel = aAnimation->mChannels[c];
            animChanel->trgNodeName = aAnimChanel->mNodeName.data;
            // Position
            animChanel->posKeyNr = aAnimChanel->mNumPositionKeys;
            animChanel->posKeyData = new Animation::AnimChanel::VectorKey[animChanel->posKeyNr];
            for (unsigned int i = 0; i < animChanel->posKeyNr; ++i) {
                Animation::AnimChanel::VectorKey* posKey = &animChanel->posKeyData[i];
                aiVectorKey* aPosKey = &aAnimChanel->mPositionKeys[i];
                posKey->time = aPosKey->mTime;
                CpyVec(posKey->value, aPosKey->mValue);
            }
            // Rotation
            animChanel->rotKeyNr = aAnimChanel->mNumRotationKeys;
            animChanel->rotKeyData = new Animation::AnimChanel::QuatKey[animChanel->rotKeyNr];
            for (unsigned int i = 0; i < animChanel->rotKeyNr; ++i) {
                Animation::AnimChanel::QuatKey* rotKey = &animChanel->rotKeyData[i];
                aiQuatKey* aRotKey = &aAnimChanel->mRotationKeys[i];
                rotKey->time = aRotKey->mTime;
                //CpyQuat(rotKey->value, aRotKey->mValue);
                rotKey->value = aRotKey->mValue;
            }
            // Scale
            animChanel->scaleKeyNr = aAnimChanel->mNumScalingKeys;
            animChanel->scaleKeyData = new Animation::AnimChanel::VectorKey[animChanel->scaleKeyNr];
            for (unsigned int i = 0; i < animChanel->scaleKeyNr; ++i) {
                Animation::AnimChanel::VectorKey* scaleKey = &animChanel->scaleKeyData[i];
                aiVectorKey* aScaleKey = &aAnimChanel->mScalingKeys[i];
                scaleKey->time = aScaleKey->mTime;
                CpyVec(scaleKey->value, aScaleKey->mValue);
            }
        }
    }
}

void RiggedModel::BoneTransform(float timeInSeconds, const Animation& animation) {
    float animationTime = 0;
    float ticksPerSecond = (float)(animation.ticksPerSecond != 0 ? animation.ticksPerSecond : 25.0f);
    float timeInTicks = timeInSeconds * ticksPerSecond;
    animationTime = fmod(timeInTicks, static_cast<float>(animation.duration));

    ReadNodeHeirarchy(animationTime, &rootNode, glm::mat4());
}

void RiggedModel::ReadNodeHeirarchy(float animationTime, Node* node, const glm::mat4& parentTransform) {
    glm::mat4 nodeTransformation(node->transformation);

    //const aiAnimation* aAnimation = tmpScene->mAnimations[0];
   // const aiNodeAnim* aNodeAnim = FindNodeAnim(pAnimation, NodeName);

    animationTime = 50;
    
    if (!animations.empty()) {
        const Animation& animation = animations[0];
        const Animation::AnimChanel* animChanel = animation.FindAnimChanel(node->name);

        if (animChanel != nullptr) {
            // Interpolate scaling and generate scaling transformation matrix.
            glm::vec3 scaling;
            CalcInterpolatedScaling(scaling, animationTime, animChanel);
            glm::mat4 scalingM(glm::scale(glm::mat4(), scaling));
            //ScalingM.InitScaleTransform(scaling.x, scaling.y, scaling.z);

            // Interpolate rotation and generate rotation transformation matrix.
            aiQuaternion rotationQ;
            CalcInterpolatedRotation(rotationQ, animationTime, animChanel);
            glm::mat4 rotationM;
            aiMatrix3x3 aMat = rotationQ.GetMatrix();
            CpyMat(rotationM, aiMatrix4x4(aMat));
            //rotationM = glm::transpose(rotationM);

            // Interpolate translation and generate translation transformation matrix.
            glm::vec3 translation;
            CalcInterpolatedPosition(translation, animationTime, animChanel);
            glm::mat4 translationM(glm::translate(glm::mat4(), translation));

            //// Combine the above transformations.
            //nodeTransformation = /*translationM * rotationM * */scalingM;
            nodeTransformation = scalingM * rotationM * glm::transpose(translationM); //TODO Check order
            bool a = true;
           // nodeTransformation = glm::transpose(nodeTransformation);
        }
    }

    glm::mat4 globalTransformation = nodeTransformation * parentTransform; //TODO Check order

    const auto& it = this->boneMap.find(node->name);
    if (it != this->boneMap.end()) {
        unsigned int boneIndex = it->second;
        this->boneInfos[boneIndex].finalTransformation = this->boneInfos[boneIndex].boneOffset * globalTransformation * this->globalInverseTransform;
    }

    for (std::size_t i = 0; i < node->children.size(); ++i) {
        ReadNodeHeirarchy(animationTime, &node->children[i], globalTransformation);
    }
}

void RiggedModel::CalcInterpolatedScaling(glm::vec3& scaling, float animationTime, const Animation::AnimChanel* animChanel) {
    // We need two values to interpolate.
    if (animChanel->scaleKeyNr == 1) {
        scaling = animChanel->scaleKeyData[0].value;
        return;
    }

    unsigned int scalingIndex = animChanel->FindScaling(animationTime);
    unsigned int nextScalingIndex = (scalingIndex + 1);
    assert(nextScalingIndex < animChanel->scaleKeyNr);
    float deltaTime = (float)(animChanel->scaleKeyData[nextScalingIndex].time - animChanel->scaleKeyData[scalingIndex].time);
    float factor = (animationTime - (float)animChanel->scaleKeyData[scalingIndex].time) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const glm::vec3& start = animChanel->scaleKeyData[scalingIndex].value;
    const glm::vec3& end = animChanel->scaleKeyData[nextScalingIndex].value;
    const glm::vec3 delta = end - start;
    scaling = start + factor * delta;
}

void RiggedModel::CalcInterpolatedRotation(aiQuaternion& rotation, float animationTime, const Animation::AnimChanel* animChanel) {
    // We need two values to interpolate.
    if (animChanel->rotKeyNr == 1) {
        rotation = animChanel->rotKeyData[0].value;
        return;
    }

    unsigned int rotationIndex = animChanel->FindRotation(animationTime);
    unsigned int nextRotationIndex = (rotationIndex + 1);
    assert(nextRotationIndex < animChanel->rotKeyNr);
    float deltaTime = (float)(animChanel->rotKeyData[nextRotationIndex].time - animChanel->rotKeyData[rotationIndex].time);
    float factor = (animationTime - (float)animChanel->rotKeyData[rotationIndex].time) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const aiQuaternion& startRotationQ = animChanel->rotKeyData[rotationIndex].value;
    const aiQuaternion& endRotationQ = animChanel->rotKeyData[nextRotationIndex].value;
    aiQuaternion::Interpolate(rotation, startRotationQ, endRotationQ, factor);//glm::mix(startRotationQ, endRotationQ, factor);
    rotation = rotation.Normalize();
}

void RiggedModel::CalcInterpolatedPosition(glm::vec3& translation, float animationTime, const Animation::AnimChanel* animChanel) {
    // We need two values to interpolate.
    if (animChanel->posKeyNr == 1) {
        translation = animChanel->posKeyData[0].value;
        return;
    }

    unsigned int positionIndex = animChanel->FindPosition(animationTime);
    unsigned int nextPositionIndex = (positionIndex + 1);
    assert(nextPositionIndex < animChanel->posKeyNr);
    float deltaTime = (float)(animChanel->posKeyData[nextPositionIndex].time - animChanel->posKeyData[positionIndex].time);
    float factor = (animationTime - (float)animChanel->posKeyData[positionIndex].time) / deltaTime;
    assert(factor >= 0.0f && factor <= 1.0f);
    const glm::vec3& start = animChanel->posKeyData[positionIndex].value;
    const glm::vec3& end = animChanel->posKeyData[nextPositionIndex].value;
    const glm::vec3 delta = end - start;
    translation = start + factor * delta;
}

void RiggedModel::MeshTransform(const std::vector<glm::mat4>& transforms) {
    // CPU-skinning.
    unsigned int boneIDs[NUM_BONES_PER_VERTEX];
    float boneWeights[NUM_BONES_PER_VERTEX];

    // Vertex shader
    for (unsigned int v = 0; v < vertices.size(); ++v) {
        float t = 0;
        for (unsigned int i = 0; i < NUM_BONES_PER_VERTEX; ++i) {
            boneIDs[i] = boneVertDatas[v].boneIDs[i];
            boneWeights[i] = boneVertDatas[v].weights[i];
            t += boneWeights[i];
        }
        if (t > 1.05f) {
            bool bad = true;
        }

        glm::mat4 boneTransform = transforms[boneIDs[0]] * boneWeights[0];
        boneTransform += transforms[boneIDs[1]] * boneWeights[1];
        boneTransform += transforms[boneIDs[2]] * boneWeights[2];
        boneTransform += transforms[boneIDs[3]] * boneWeights[3];

        boneTransform = glm::transpose(boneTransform);
        VertexType::SkinVertex& vert = vertices[v];
        vert.position = boneTransform * glm::vec4(vert.position, 1.f);
        vert.normal = boneTransform * glm::vec4(vert.normal, 0.f);
    }
}

void RiggedModel::NodeTransform(const Node* node, glm::mat4& transform) {
    if (node->parent) {
        NodeTransform(node->parent, transform);
        transform = transform * node->transformation;
    }
    else {
        transform = node->transformation;
    }
}
