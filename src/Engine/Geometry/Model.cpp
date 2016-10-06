#include "Model.hpp"

#include "../Util/Log.hpp"
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"
#include <assimp/cimport.h> // aiMultiplyMatrix4
#include <glm/gtc/matrix_transform.hpp>
#include <map>

using namespace Geometry;

Assimp::Importer Model::aiImporter = Assimp::Importer();

Model::Model() {

}

Model::Model(const char* filename) {
    Load(filename);
}

Model::~Model() {
    animation.Clear();
    skeleton.Clear();
}

Geometry3D::Vertex* Model::GetVertices() const {
    return vertexData;
}

unsigned int Model::GetVertexCount() const {
    return vertexNr;
}

unsigned int* Model::GetIndices() const {
    return indexData;
}

unsigned int Model::GetIndexCount() const {
    return indexNr;
}

Json::Value Model::Save() const {
    Json::Value model;
    model["name"] = name;
    return model;
}

void Model::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    Load((Hymn().GetPath() + FileSystem::DELIMITER + "Models" + FileSystem::DELIMITER + name + ".fbx").c_str());
}

void Model::Load(const char* filename) {
    const aiScene* scene = aiImporter.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace | 0);

    assert(scene->HasMeshes());
    if (scene->mNumMeshes > 1) {
        Log() << "Warning loading model: " << filename << " contains more then one mesh. Only one mesh will be loaded.\n";
    }

    // TMP
    tmpAssimpMesh = scene->mMeshes[0];
    //tmpAssimpAnimation = scene->HasAnimations() ? scene->mAnimations[0] : nullptr;
    //tmpRootNode = scene->mRootNode;

    aiMesh* assimpMesh = scene->mMeshes[0];

    LoadMesh(assimpMesh, mesh);
    if (assimpMesh->HasBones())
        LoadSkeleton(scene->mRootNode, assimpMesh, skeleton);
    if (scene->HasAnimations()) 
        LoadAnimation(scene->mAnimations[0], animation);

    // Importing the Animations
    if (scene->HasAnimations()) {
        //TransfromMesh(skeleton);
        //float animtick = 2;
        //float timestep = 1.f/60.f;
        //float animfps = 1;
        //animtick = animtick + (timestep / 1000.f) * animfps;
        BindAnimation(skeleton, animation);
        AnimateSkeleton(animation, 2.4f);
        TransfromMesh(skeleton);
        //AninmateMesh(animation.assimpAnimation, 1, mesh);
    }
    // ~TMP

    // --- Generate vertex buffer ---
    vertexData = mesh.vertexData;
    vertexNr = mesh.vertexNr;
    indexData = mesh.indexData;
    indexNr = mesh.indexNr;

    GenerateBuffers();
    GenerateVertexArray();

    mesh.Clear();

    // TODO IMPORTER ISN'T THREAD-SAFE
    aiImporter.FreeScene();
}

void Model::LoadMesh(aiMesh* assimpMesh, Mesh& mesh) {
    mesh.vertexNr = assimpMesh->mNumVertices;
    mesh.vertexData = new Vertex[mesh.vertexNr];
    mesh.indexNr = assimpMesh->mNumFaces * 3;
    mesh.indexData = new unsigned int[mesh.indexNr];

    for (unsigned int i = 0; i < mesh.vertexNr; ++i) {
        CpyVec(assimpMesh->mVertices[i], mesh.vertexData[i].position);
        CpyVec(assimpMesh->mTextureCoords[0][i], mesh.vertexData[i].textureCoordinate);
        CpyVec(assimpMesh->mNormals[i], mesh.vertexData[i].normal);
        CpyVec(assimpMesh->mTangents[i], mesh.vertexData[i].tangent);
    }

    for (unsigned int i = 0; i < assimpMesh->mNumFaces; ++i) {
        mesh.indexData[i * 3] = assimpMesh->mFaces[i].mIndices[0];
        mesh.indexData[i * 3 + 1] = assimpMesh->mFaces[i].mIndices[1];
        mesh.indexData[i * 3 + 2] = assimpMesh->mFaces[i].mIndices[2];
    }
}

void Model::LoadSkeleton(aiNode* rootNode, aiMesh* assimpMesh, Skeleton& skeleton) {
    // Load bone names
    std::map<std::string, aiBone*> boneMap;
    for (unsigned int i = 0; i < assimpMesh->mNumBones; ++i) {
        boneMap[assimpMesh->mBones[i]->mName.data] = assimpMesh->mBones[i];
    }

    // Allocate joints
    skeleton.jointNr = assimpMesh->mNumBones;
    skeleton.jointData = new Skeleton::Joint[skeleton.jointNr];

    // Build skeleton
    skeleton.rootJoint = &skeleton.jointData[0];
    unsigned int index = 0;
    skeleton.BuildSkeleton(rootNode, 0, index, boneMap);
}

void Model::LoadAnimation(aiAnimation* assimpAnimation, Animation& animation) {
    // Load animation
    animation.animChanelNr = assimpAnimation->mNumChannels;
    animation.animChanelData = new Animation::AnimChanel[animation.animChanelNr];
    for (unsigned int a = 0; a < animation.animChanelNr; ++a) {
        Animation::AnimChanel* animChanel = &animation.animChanelData[a];
        aiNodeAnim* assimpAnimChanel = assimpAnimation->mChannels[a];
        animChanel->jointName = assimpAnimChanel->mNodeName.data;
        animChanel->rotKeyNr = assimpAnimChanel->mNumRotationKeys;
        animChanel->rotKeyData = new Animation::AnimChanel::QuatKey[animChanel->rotKeyNr];
        for (unsigned int i = 0; i < animChanel->rotKeyNr; ++i) {
            Animation::AnimChanel::QuatKey* rotKey = &animChanel->rotKeyData[i];
            aiQuatKey* assimpRotKey = &assimpAnimChanel->mRotationKeys[i];
            rotKey->time = assimpRotKey->mTime;
            CpyQuat(assimpRotKey->mValue, rotKey->quat);
        }
    }
}

void Model::BindAnimation(Skeleton& skeleton, Animation& animation) {
    // Bind animation
    for (unsigned int i = 0; i < animation.animChanelNr; ++i) {
        Animation::AnimChanel* animChanel = &animation.animChanelData[i];
        animChanel->jointID = skeleton.FindJoint(animChanel->jointName)->myID; // TODO not use FindJoint
    }
}

void Model::TransfromMesh(const Skeleton& skeleton) {
    // Collapse mesh
    for (unsigned int i = 0; i < mesh.vertexNr; ++i) {
        mesh.vertexData[i].position = glm::vec3(0.f, 0.f, 0.f);
        mesh.vertexData[i].normal = glm::vec3(0.f, 0.f, 0.f);
    }

    TransformNode(skeleton.rootJoint, skeleton.rootJoint->transformation);
}

void Model::TransformNode(Skeleton::Joint* currentJoint, glm::mat4 transfromMatrix) {
    // TODO CHECK ORDER OF MATRIX MULTI.

    const Skeleton::Joint::Bone* bone = &currentJoint->bone;
    glm::mat3 skinMatrix = bone->offsetMatrix * transfromMatrix;
    for (unsigned int i = 0; i < bone->weightNr; ++i) {
        unsigned int vID = bone->weightData[i].vID;
        float weight = bone->weightData[i].weight;

        // TMP
        aiVector3D orgPos = tmpAssimpMesh->mVertices[vID];
        glm::vec3 position; 
        CpyVec(orgPos, position);
        //glm::vec3 position = mesh.vertexData[vID].position;
        position = skinMatrix * position;
        mesh.vertexData[vID].position.x += position.x * weight;
        mesh.vertexData[vID].position.y += position.y * weight;
        mesh.vertexData[vID].position.z += position.z * weight;

        // TMP
        aiVector3D orgNorm = tmpAssimpMesh->mNormals[vID];
        glm::vec3 normal; 
        CpyVec(orgNorm, normal);
        //glm::vec3 normal = mesh.vertexData[vID].normal;
        normal = skinMatrix * normal;
        mesh.vertexData[vID].normal.x += normal.x * weight;
        mesh.vertexData[vID].normal.y += normal.y * weight;
        mesh.vertexData[vID].normal.z += normal.z * weight;
    }
    for (unsigned int i = 0; i < currentJoint->nrChildren; ++i) {
        Skeleton::Joint* childJoint = &skeleton.jointData[currentJoint->myID + 1 + i];
        TransformNode(childJoint, transfromMatrix * childJoint->transformation);
    }
}

void Model::AnimateSkeleton(const Animation& animation, const float tick) {
    const int frame = static_cast<int>(std::floor(tick));
    const float t = tick - frame;

    for (unsigned int i = 0; i < animation.animChanelNr; ++i) {
        Animation::AnimChanel* animChanel = &animation.animChanelData[i];
        Skeleton::Joint* joint = &skeleton.jointData[animChanel->jointID];
        Animation::AnimChanel::QuatKey* r0 = animChanel->rotKeyData + (frame + 0) % animChanel->rotKeyNr;
        Animation::AnimChanel::QuatKey* r1 = animChanel->rotKeyData + (frame + 1) % animChanel->rotKeyNr;
        glm::quat r;
        Mix(r0->quat, r1->quat, t, r);
        ComposeMatrix(r, joint->transformation);
    }
}

void Model::Skeleton::BuildSkeleton(aiNode* assimpNode, unsigned int pID, unsigned int& index, const std::map<std::string, aiBone*>& boneMap) {
    auto it = boneMap.find(assimpNode->mName.data);
    if (it != boneMap.end()) {
        // Bone found
        Skeleton::Joint* joint = &jointData[index];
        // Cpy bone to joint
        aiBone* assimpBone = it->second;
        Skeleton::Joint::Bone* bone = &joint->bone;
        CpyMat(assimpBone->mOffsetMatrix, bone->offsetMatrix);
        bone->weightNr = assimpBone->mNumWeights;
        bone->weightData = new Skeleton::Joint::Bone::VertexWeight[bone->weightNr];
        for (unsigned int w = 0; w < bone->weightNr; ++w) {
            Skeleton::Joint::Bone::VertexWeight* vertexWeight = &bone->weightData[w];
            aiVertexWeight* assimpWeight = &assimpBone->mWeights[w];
            vertexWeight->vID = assimpWeight->mVertexId;
            vertexWeight->weight = assimpWeight->mWeight;
        }
        // Joint info
        joint->name = assimpBone->mName.data;
        joint->myID = index;
        joint->pID = pID;

        index++;
        unsigned int lastIndex = index;
        for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i) {
            BuildSkeleton(assimpNode->mChildren[i], joint->myID, index, boneMap);
            if (lastIndex != index) {
                joint->nrChildren++;
            }
        }
    }
    else {
        // Continue searching for bones
        for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i) {
            BuildSkeleton(assimpNode->mChildren[i], pID, index, boneMap);
        }
    }
}

Model::Skeleton::Joint* Model::Skeleton::FindJoint(const char* name) {
    for (unsigned int i = 0; i < jointNr; ++i)
        if (!std::strcmp(jointData[i].name, name))
            return &jointData[i];
    return nullptr;
}

void Model::Mesh::Clear() {
    if (vertexData != nullptr) {
        delete[] vertexData;
        vertexData = nullptr;
        vertexNr = 0;
    }
    if (indexData != nullptr) {
        indexData = nullptr;
        indexNr = 0;
    }
}

void Model::Skeleton::Clear() {
    for (unsigned int j = 0; j < jointNr; ++j) {
        Skeleton::Joint::Bone* bone = &jointData[j].bone;
        bone->Clear();
    }
    delete[] jointData;
    jointData = nullptr;
    jointNr = 0;
}


void Model::Skeleton::Joint::Bone::Clear() {
    if (weightData != nullptr) {
        delete[] weightData;
        weightData = nullptr;
        weightNr = 0;
    }
}

void Model::Animation::Clear() {

}

//void Model::Mix(aiQuaternion& q1, const aiQuaternion& q2, float t, aiQuaternion& result) {
//    aiQuaternion tmp;
//    if (Dot(q1, q2) < 0) {
//        tmp.x = -q1.x;
//        tmp.y = -q1.y;
//        tmp.z = -q1.z;
//        tmp.w = -q1.w;
//        q1 = tmp;
//    }
//    result.x = q1.x + t * (q2.x - q1.x);
//    result.y = q1.y + t * (q2.y - q1.y);
//    result.z = q1.z + t * (q2.z - q1.z);
//    result.w = q1.w + t * (q2.w - q1.w);
//    Normalize(result);
//}
//
//float Model::Dot(const aiQuaternion& q1, const aiQuaternion& q2) {
//    return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
//}
//
//void Model::Normalize(aiQuaternion& q) {
//    float d = sqrt(Dot(q, q));
//    if (d >= 0.00001) {
//        d = 1 / d;
//        q.x *= d;
//        q.y *= d;
//        q.z *= d;
//        q.w *= d;
//    }
//    else {
//        q.x = q.y = q.z = 0;
//        q.w = 1;
//    }
//}
//
//void Model::ComposeMatrix(const aiQuaternion& q, aiMatrix4x4& m) {
//    // Rotation
//    m.a1 = 1 - 2 * (q.y * q.y + q.z * q.z);
//    m.a2 = 2 * (q.x * q.y - q.z * q.w);
//    m.a3 = 2 * (q.x * q.z + q.y * q.w);
//    m.b1 = 2 * (q.x * q.y + q.z * q.w);
//    m.b2 = 1 - 2 * (q.x * q.x + q.z * q.z);
//    m.b3 = 2 * (q.y * q.z - q.x * q.w);
//    m.c1 = 2 * (q.x * q.z - q.y * q.w);
//    m.c2 = 2 * (q.y * q.z + q.x * q.w);
//    m.c3 = 1 - 2 * (q.x * q.x + q.y * q.y);
//
//    // TODO Scale
//
//    // TODO Translation
//
//    m.d1 = 0; m.d2 = 0; m.d3 = 0; m.d4 = 1;
//}

void Model::Mix(glm::quat& q1, const glm::quat& q2, float t, glm::quat& result) {
    glm::quat tmp;
    if (Dot(q1, q2) < 0) {
        tmp.x = -q1.x;
        tmp.y = -q1.y;
        tmp.z = -q1.z;
        tmp.w = -q1.w;
        q1 = tmp;
    }
    result.x = q1.x + t * (q2.x - q1.x);
    result.y = q1.y + t * (q2.y - q1.y);
    result.z = q1.z + t * (q2.z - q1.z);
    result.w = q1.w + t * (q2.w - q1.w);
    Normalize(result);
}

float Model::Dot(const glm::quat& q1, const glm::quat& q2) {
    return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
}

void Model::Normalize(glm::quat& q) {
    float d = sqrt(Dot(q, q));
    if (d >= 0.00001) {
        d = 1 / d;
        q.x *= d;
        q.y *= d;
        q.z *= d;
        q.w *= d;
    }
    else {
        q.x = q.y = q.z = 0;
        q.w = 1;
    }
}

void Model::ComposeMatrix(const glm::quat& q, glm::mat4& m) {
    // Rotation
    m[0][0] = 1.f - 2.f * (q.y * q.y + q.z * q.z);
    m[0][1] = 2.f * (q.x * q.y - q.z * q.w);
    m[0][2] = 2.f * (q.x * q.z + q.y * q.w);
    m[1][0] = 2.f * (q.x * q.y + q.z * q.w);
    m[1][1] = 1.f - 2.f * (q.x * q.x + q.z * q.z);
    m[1][2] = 2.f * (q.y * q.z - q.x * q.w);
    m[2][0] = 2.f * (q.x * q.z - q.y * q.w);
    m[2][1] = 2.f * (q.y * q.z + q.x * q.w);
    m[2][2] = 1.f - 2.f * (q.x * q.x + q.y * q.y);

    // TODO Scale

    // TODO Translation

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
}

//aiNode* Model::FindNode(aiNode* node, const char* name) {
//    if (!std::strcmp(name, node->mName.data))
//        return node;
//    for (unsigned int i = 0; i < node->mNumChildren; i++) {
//        aiNode* found = FindNode(node->mChildren[i], name);
//        if (found)
//            return found;
//    }
//    return nullptr;
//}
//
//void Model::TransformNode(const aiNode* node, aiMatrix4x4& transformMat) {
//    if (node->mParent) {
//        TransformNode(node->mParent, transformMat);
//        aiMultiplyMatrix4(&transformMat, &node->mTransformation);
//    }
//    else {
//        transformMat = node->mTransformation;
//    }
//}

void Model::CpyVec(const aiVector3D& aiVec, glm::vec3& glmVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
    glmVec.z = aiVec.z;
}

void Model::CpyVec(const aiVector3D& aiVec, glm::vec2& glmVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
}


void Model::CpyVec(const aiVector2D& aiVec, glm::vec2& glmVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
}

void Model::CpyMat(const aiMatrix4x4& aiMat, glm::mat4& glmMat) {
    glmMat[0][0] = aiMat.a1;
    glmMat[0][1] = aiMat.a2;
    glmMat[0][2] = aiMat.a3;
    glmMat[0][3] = aiMat.a4;

    glmMat[1][0] = aiMat.b1;
    glmMat[1][1] = aiMat.b2;
    glmMat[1][2] = aiMat.b3;
    glmMat[1][3] = aiMat.b4;

    glmMat[2][0] = aiMat.c1;
    glmMat[2][1] = aiMat.c2;
    glmMat[2][2] = aiMat.c3;
    glmMat[2][3] = aiMat.c4;

    glmMat[3][0] = aiMat.d1;
    glmMat[3][1] = aiMat.d2;
    glmMat[3][2] = aiMat.d3;
    glmMat[3][3] = aiMat.d4;
}

void Model::CpyMat(const aiMatrix4x4& aiMat4, aiMatrix3x3& aiMat3) {
    aiMat3.a1 = aiMat4.a1;
    aiMat3.a2 = aiMat4.a2;
    aiMat3.a3 = aiMat4.a3;

    aiMat3.b1 = aiMat4.b1;
    aiMat3.b2 = aiMat4.b2;
    aiMat3.b3 = aiMat4.b3;

    aiMat3.c1 = aiMat4.c1;
    aiMat3.c2 = aiMat4.c2;
    aiMat3.c3 = aiMat4.c3;
}

void Model::CpyQuat(const aiQuaternion& aiQuat, glm::quat &glmQuat) {
    glmQuat.x = aiQuat.x;
    glmQuat.y = aiQuat.y;
    glmQuat.z = aiQuat.z;
    glmQuat.w = aiQuat.w;
}

//void Model::TransfromMesh(aiNode* rootNode, Mesh& mesh) {
//    const aiMesh* assimpMesh = mesh.assimpMesh;
//    if (!assimpMesh->HasBones())
//        return;
//
//    // Collapse mesh
//    for (unsigned int i = 0; i < mesh.vertexNr; ++i) {
//        mesh.vertexData[i].position = glm::vec3(0.f, 0.f, 0.f);
//        mesh.vertexData[i].normal = glm::vec3(0.f, 0.f, 0.f);
//    }
//
//    aiMatrix4x4 skin4x4Mat;
//    aiMatrix3x3 skin3x3Mat;
//
//    for (unsigned int b = 0; b < assimpMesh->mNumBones; ++b) {
//        const aiBone* bone = assimpMesh->mBones[b];
//        const aiNode* node = FindNode(rootNode, bone->mName.data); // Find node corresponding to this bone
//
//        TransformNode(node, skin4x4Mat); // Transform bone matrix
//        aiMultiplyMatrix4(&skin4x4Mat, &bone->mOffsetMatrix);
//        CpyMat(skin4x4Mat, skin3x3Mat); // Extract 3x3 matrix from 4x4 matrix.
//
//        for (unsigned int i = 0; i < bone->mNumWeights; ++i) {
//            unsigned int vId = bone->mWeights[i].mVertexId;
//            float weight = bone->mWeights[i].mWeight;
//
//            aiVector3D position = assimpMesh->mVertices[vId];
//            aiTransformVecByMatrix4(&position, &skin4x4Mat);
//            mesh.vertexData[vId].position.x += position.x * weight;
//            mesh.vertexData[vId].position.y += position.y * weight;
//            mesh.vertexData[vId].position.z += position.z * weight;
//
//            aiVector3D normal = assimpMesh->mNormals[vId];
//            aiTransformVecByMatrix3(&normal, &skin3x3Mat);
//            mesh.vertexData[vId].normal.x += normal.x * weight;
//            mesh.vertexData[vId].normal.y += normal.y * weight;
//            mesh.vertexData[vId].normal.z += normal.z * weight;
//        }
//    }
//}

//void Model::AninmateMesh(const aiAnimation* animation, const float tick, Mesh& mesh) {
//    const int frame = static_cast<int>(std::floor(tick));
//    const float t = tick - frame;
//
//    aiQuaternion r;
//    for (unsigned int i = 0; i < animation->mNumChannels; ++i) {
//        const aiNodeAnim* channel = animation->mChannels[i];
//        aiNode* node = FindNode(mesh.rootNode, channel->mNodeName.data);
//        aiQuatKey* r0 = channel->mRotationKeys + (frame + 0) % channel->mNumRotationKeys;
//        aiQuatKey* r1 = channel->mRotationKeys + (frame + 1) % channel->mNumRotationKeys;
//        Mix(r0->mValue, r1->mValue, t, r);
//        ComposeMatrix(r, node->mTransformation);
//    }
//
//    TransfromMesh(mesh);
//}
