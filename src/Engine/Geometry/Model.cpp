#include "Model.hpp"

#include "../Util/Log.hpp"
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"
#include <assimp/cimport.h> // aiMultiplyMatrix4
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <glm/gtc/type_ptr.hpp>

using namespace Geometry;

Assimp::Importer Model::aiImporter = Assimp::Importer();

Model::Model() {

}

Model::Model(const char* filename) {
    Load(filename);
}

Model::~Model() {
    //animation.Clear();
    //skeleton.Clear();
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
    model["extension"] = extension;
    return model;
}

void Model::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    extension = node.get("extension", "").asString();
    Load((Hymn().GetPath() + FileSystem::DELIMITER + "Models" + FileSystem::DELIMITER + name + "." + extension).c_str());
}

void Model::Load(const char* filename) {
    const aiScene* aScene = aiImporter.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality);

    assert(aScene != nullptr);

    CpyMat(this->globalInverseTransform, aScene->mRootNode->mTransformation);
    this->globalInverseTransform = glm::inverse(this->globalInverseTransform);

    LoadMeshes(aScene);
    LoadNodeTree(aScene->mRootNode, &rootNode, nullptr);
    
    // TMP
    tmpScene = aScene;

    std::vector<glm::mat4> transforms;
    BoneTransform(0.f, transforms);

    MeshTransform(transforms);

    //tmpAssimpMesh = scene->mMeshes[0];
    //tmpAssimpAnimation = scene->HasAnimations() ? scene->mAnimations[0] : nullptr;
    ////tmpRootNode = scene->mRootNode;

    //aiMesh* assimpMesh = scene->mMeshes[0]; // TODO load all meshes

    //LoadMesh(assimpMesh, mesh);
    //if (assimpMesh->HasBones())
    //    LoadSkeleton(scene->mRootNode, assimpMesh, skeleton);
    //if (scene->HasAnimations()) 
    //    LoadAnimation(scene->mAnimations[0], animation);

    //// Importing the Animations
    //if (scene->HasAnimations()) {
    //    TransfromMesh(skeleton);
    //    //float animtick = 2;
    //    //float timestep = 1.f/60.f;
    //    //float animfps = 1;
    //    //animtick = animtick + (timestep / 1000.f) * animfps;
    //    AnimateSkeleton(animation, 0.1f);
    //    TransfromMesh(skeleton);
    //    //AninmateMesh(animation.assimpAnimation, 1, mesh);
    //}
    // ~TMP

    // --- Generate vertex buffer ---
    //vertexData = mesh.vertexData;
    //vertexNr = mesh.vertexNr;
    //indexData = mesh.indexData;
    //indexNr = mesh.indexNr;

    GenerateBuffers();
    GenerateVertexArray();

    //mesh.Clear();
    delete[] this->vertexData;
    delete[] this->indexData;

    aiImporter.FreeScene();
}

void Model::LoadMeshes(const aiScene* aScene) {
    entries.resize(aScene->mNumMeshes);

    unsigned int numVertices = 0;
    unsigned int numIndices = 0;

    // Count the number of vertices and indices.
    for (unsigned int i = 0; i < aScene->mNumMeshes; ++i) {
        entries[i].numIndices = aScene->mMeshes[i]->mNumFaces * 3;
        entries[i].baseVertex = numVertices;
        entries[i].baseIndex = numIndices;

        numVertices += aScene->mMeshes[i]->mNumVertices;
        numIndices += entries[i].numIndices;
    }

    // Allocate memory for the vertices and indices.
    this->vertexNr = numVertices;
    this->vertexData = new Vertex[numVertices]; //TODO delete
    this->indexNr = numIndices;
    this->indexData = new unsigned int[numIndices]; //TODO delete
    // Resize bones vector
    boneVertDatas.resize(numVertices);

    numVertices = 0;
    numIndices = 0;

    // Initialize the meshes in the scene one by one.
    for (unsigned int m = 0; m < aScene->mNumMeshes; ++m) {
        const aiMesh* aMesh = aScene->mMeshes[m];
        // Load vertices.
        for (unsigned int i = 0; i < aMesh->mNumVertices; ++i) {
            Vertex& vert = this->vertexData[numVertices++];
            CpyVec(vert.position, aMesh->mVertices[i]);
            CpyVec(vert.textureCoordinate, aMesh->mTextureCoords[0][i]);
            CpyVec(vert.normal, aMesh->mNormals[i]);
            CpyVec(vert.tangent, aMesh->mTangents[i]);
        }
        // Load indices.
        for (unsigned int i = 0; i < aMesh->mNumFaces; ++i) {
            const aiFace& aFace = aMesh->mFaces[i];
            assert(aFace.mNumIndices == 3);
            this->indexData[numIndices++] = aFace.mIndices[0];
            this->indexData[numIndices++] = aFace.mIndices[1];
            this->indexData[numIndices++] = aFace.mIndices[2];
        }
        // Load bones.
        for (unsigned int b = 0; b < aMesh->mNumBones; ++b) {
            const aiBone* aBone = aMesh->mBones[b];
            unsigned int boneIndex = 0;
            std::string boneName(aBone->mName.data);

            // Check in bone map if it's a new bone.
            auto& it = this->boneMap.find(boneName);
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
            for (unsigned int i = 0; i < aBone->mNumWeights; i++) {
                unsigned int vertexID = entries[m].baseVertex + aBone->mWeights[i].mVertexId;
                float weight = aBone->mWeights[i].mWeight;
                boneVertDatas[vertexID].AddBoneData(boneIndex, weight);
            }
        }
    }
}

void Model::BoneTransform(float timeInSeconds, std::vector<glm::mat4>& transforms) {
    float animationTime = 0;
    if (tmpScene->HasAnimations()) {
        float ticksPerSecond = (float)(tmpScene->mAnimations[0]->mTicksPerSecond != 0 ? tmpScene->mAnimations[0]->mTicksPerSecond : 25.0f);
        float timeInTicks = timeInSeconds * ticksPerSecond;
        float animationTime = fmod(timeInTicks, static_cast<float>(tmpScene->mAnimations[0]->mDuration));
    }

    ReadNodeHeirarchy(animationTime, &rootNode, glm::mat4());

    transforms.resize(this->numBones);

    for (unsigned int i = 0; i < this->numBones; i++) {
        transforms[i] = this->boneInfos[i].finalTransformation;
    }
}

void Model::LoadNodeTree(aiNode* aNode, Node* node, Node* parentNode) {
    node->name = aNode->mName.C_Str();
    CpyMat(node->transformation, aNode->mTransformation);
    node->parent = parentNode;
    node->children.resize(aNode->mNumChildren);
    for (std::size_t i = 0; i < aNode->mNumChildren; ++i) {
        LoadNodeTree(aNode->mChildren[i], &node->children[i], node);
    }
}

void Model::ReadNodeHeirarchy(float animationTime, Node* node, const glm::mat4& parentTransform) {
    std::string nodeName(node->name);

    glm::mat4 nodeTransformation(node->transformation);

    //const aiAnimation* aAnimation = tmpScene->mAnimations[0];
    //const aiNodeAnim* aNodeAnim = FindNodeAnim(pAnimation, NodeName);

    //if (aNodeAnim) {
    //    // Interpolate scaling and generate scaling transformation matrix
    //    aiVector3D Scaling;
    //    CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
    //    Matrix4f ScalingM;
    //    ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

    //    // Interpolate rotation and generate rotation transformation matrix
    //    aiQuaternion RotationQ;
    //    CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
    //    Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());

    //    // Interpolate translation and generate translation transformation matrix
    //    aiVector3D Translation;
    //    CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
    //    Matrix4f TranslationM;
    //    TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

    //    // Combine the above transformations
    //    NodeTransformation = TranslationM * RotationM * ScalingM;
    //}

    glm::mat4 globalTransformation = parentTransform * nodeTransformation; // TODO Check order of transform

    auto& it = this->boneMap.find(nodeName);
    if (it != this->boneMap.end()) {
        unsigned int boneIndex = it->second;
        this->boneInfos[boneIndex].finalTransformation = this->globalInverseTransform * globalTransformation * this->boneInfos[boneIndex].boneOffset;
    }

    for (std::size_t i = 0; i < node->children.size(); ++i) {
        ReadNodeHeirarchy(animationTime, &node->children[i], globalTransformation);
    }

    //for (unsigned int i = 0; i < aNode->mNumChildren; ++i) {
    //    ReadNodeHeirarchy(animationTime, aNode->mChildren[i], globalTransformation);
    //}
}

void Model::MeshTransform(const std::vector<glm::mat4>& transforms) {
    // CPU-skinning.
    unsigned int boneIDs[NUM_BONES_PER_VERTEX];
    float boneWeights[NUM_BONES_PER_VERTEX];

    // Vertex shader
    for (unsigned int v = 0; v < this->vertexNr; ++v) {
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

        Vertex& vert = this->vertexData[v];
        vert.position = boneTransform * glm::vec4(vert.position, 1.f);
        vert.normal = boneTransform * glm::vec4(vert.normal, 0.f);
    }
}

//void Model::LoadMesh(aiMesh* assimpMesh, Mesh& mesh) {
//    mesh.vertexNr = assimpMesh->mNumVertices;
//    mesh.vertexData = new Vertex[mesh.vertexNr];
//    mesh.indexNr = assimpMesh->mNumFaces * 3;
//    mesh.indexData = new unsigned int[mesh.indexNr];
//
//    for (unsigned int i = 0; i < mesh.vertexNr; ++i) {
//        CpyVec(assimpMesh->mVertices[i], mesh.vertexData[i].position);
//        CpyVec(assimpMesh->mTextureCoords[0][i], mesh.vertexData[i].textureCoordinate);
//        CpyVec(assimpMesh->mNormals[i], mesh.vertexData[i].normal);
//        CpyVec(assimpMesh->mTangents[i], mesh.vertexData[i].tangent);
//    }
//
//    for (unsigned int i = 0; i < assimpMesh->mNumFaces; ++i) {
//        mesh.indexData[i * 3] = assimpMesh->mFaces[i].mIndices[0];
//        mesh.indexData[i * 3 + 1] = assimpMesh->mFaces[i].mIndices[1];
//        mesh.indexData[i * 3 + 2] = assimpMesh->mFaces[i].mIndices[2];
//    }
//}
//
//void Model::LoadSkeleton(aiNode* rootNode, aiMesh* assimpMesh, Skeleton& skeleton) {
//    skeleton.Load(rootNode, assimpMesh);
//}
//
//void Model::LoadAnimation(aiAnimation* assimpAnimation, Animation& animation) {
//    // Load animation
//    animation.animChanelNr = assimpAnimation->mNumChannels;
//    animation.animChanelData = new Animation::AnimChanel[animation.animChanelNr];
//    for (unsigned int a = 0; a < animation.animChanelNr; ++a) {
//        Animation::AnimChanel* animChanel = &animation.animChanelData[a];
//        aiNodeAnim* assimpAnimChanel = assimpAnimation->mChannels[a];
//        animChanel->trgNodeName = assimpAnimChanel->mNodeName.data;
//        // Position
//        animChanel->posKeyNr = assimpAnimChanel->mNumPositionKeys;
//        animChanel->posKeyData = new Animation::AnimChanel::VectorKey[animChanel->posKeyNr];
//        for (unsigned int i = 0; i < animChanel->posKeyNr; ++i) {
//            Animation::AnimChanel::VectorKey* posKey = &animChanel->posKeyData[i];
//            aiVectorKey* assimpPosKey = &assimpAnimChanel->mPositionKeys[i];
//            posKey->time = assimpPosKey->mTime;
//            CpyVec(assimpPosKey->mValue, posKey->value);
//        }
//        // Rotation
//        animChanel->rotKeyNr = assimpAnimChanel->mNumRotationKeys;
//        animChanel->rotKeyData = new Animation::AnimChanel::QuatKey[animChanel->rotKeyNr];
//        for (unsigned int i = 0; i < animChanel->rotKeyNr; ++i) {
//            Animation::AnimChanel::QuatKey* rotKey = &animChanel->rotKeyData[i];
//            aiQuatKey* assimpRotKey = &assimpAnimChanel->mRotationKeys[i];
//            rotKey->time = assimpRotKey->mTime;
//            CpyQuat(assimpRotKey->mValue, rotKey->value);
//        }
//        // Scale
//        animChanel->scaleKeyNr = assimpAnimChanel->mNumScalingKeys;
//        animChanel->scaleKeyData = new Animation::AnimChanel::VectorKey[animChanel->scaleKeyNr];
//        for (unsigned int i = 0; i < animChanel->scaleKeyNr; ++i) {
//            Animation::AnimChanel::VectorKey* scaleKey = &animChanel->scaleKeyData[i];
//            aiVectorKey* assimpScaleKey = &assimpAnimChanel->mScalingKeys[i];
//            scaleKey->time = assimpScaleKey->mTime;
//            CpyVec(assimpScaleKey->mValue, scaleKey->value);
//        }
//    }
//}
//
//void Model::TransfromMesh(Skeleton& skeleton) {
//    // Collapse mesh
//    for (unsigned int i = 0; i < mesh.vertexNr; ++i) {
//        mesh.vertexData[i].position = glm::vec3(0.f, 0.f, 0.f);
//        mesh.vertexData[i].normal = glm::vec3(0.f, 0.f, 0.f);
//    }
//
//    for (auto& it : skeleton.boneMap) {
//        const Skeleton::Bone* bone = &it.second;
//        const Skeleton::Node* node = skeleton.FindNode(bone->name.c_str());
//
//		glm::mat4 globalTranformMatrix;
//        TransformNode(node, globalTranformMatrix);
//        glm::mat3 skinMatrix(bone->offsetMatrix * globalTranformMatrix); // CHECK ORDER
//
//        for (unsigned int i = 0; i < bone->weightNr; ++i) {
//            unsigned int vID = bone->weightData[i].vID;
//            float weight = bone->weightData[i].weight;
//
//            // TMP
//            aiVector3D orgPos = tmpAssimpMesh->mVertices[vID];
//            glm::vec3 position;
//            CpyVec(orgPos, position);
//            //glm::vec3 position = mesh.vertexData[vID].position;
//            position = skinMatrix * position;
//            mesh.vertexData[vID].position.x += position.x * weight;
//            mesh.vertexData[vID].position.y += position.y * weight;
//            mesh.vertexData[vID].position.z += position.z * weight;
//
//            // TMP
//            aiVector3D orgNorm = tmpAssimpMesh->mNormals[vID];
//            glm::vec3 normal;
//            CpyVec(orgNorm, normal);
//            //glm::vec3 normal = mesh.vertexData[vID].normal;
//            normal = skinMatrix * normal;
//            mesh.vertexData[vID].normal.x += normal.x * weight;
//            mesh.vertexData[vID].normal.y += normal.y * weight;
//            mesh.vertexData[vID].normal.z += normal.z * weight;
//        }
//    }
//}
//
//void Model::TransformNode(const Skeleton::Node* node, glm::mat4& transfromMatrix) {
//    if (node->parent) {
//        TransformNode(node->parent, transfromMatrix);
//        transfromMatrix = node->transformation * transfromMatrix; // TODO CHECK ORDER OF MATRIX MULTI
//    } else {
//        transfromMatrix = node->transformation;
//    }
//}
//
//void Model::AnimateSkeleton(const Animation& animation, const float tick) {
//    const int frame = static_cast<int>(std::floor(tick));
//    const float t = tick - frame;
//
//    for (unsigned int i = 0; i < animation.animChanelNr; ++i) {
//        Animation::AnimChanel* animChanel = &animation.animChanelData[i];
//        Skeleton::Node* node = this->skeleton.FindNode(animChanel->trgNodeName.c_str());
//        Animation::AnimChanel::VectorKey* p0 = animChanel->posKeyData + (frame + 0) % animChanel->posKeyNr;
//        Animation::AnimChanel::VectorKey* p1 = animChanel->posKeyData + (frame + 1) % animChanel->posKeyNr;
//        Animation::AnimChanel::QuatKey* r0 = animChanel->rotKeyData + (frame + 0) % animChanel->rotKeyNr;
//        Animation::AnimChanel::QuatKey* r1 = animChanel->rotKeyData + (frame + 1) % animChanel->rotKeyNr;
//        Animation::AnimChanel::VectorKey* s0 = animChanel->scaleKeyData + (frame + 0) % animChanel->scaleKeyNr;
//        Animation::AnimChanel::VectorKey* s1 = animChanel->scaleKeyData + (frame + 1) % animChanel->scaleKeyNr;
//        
//        // Mix position
//        glm::vec3 p;
//        MixVec(p0->value, p1->value, t, p);
//        // Mix rotation
//        glm::quat r;
//        MixQuat(r0->value, r1->value, t, r);
//        // Mix position
//        glm::vec3 s;
//        MixVec(s0->value, s1->value, t, s);
//		glm::mat4 transform;
//        ComposeMatrix(p, r, s, transform);
//
//		node->transformation = transform;
//    }
//}
//
//void Model::Skeleton::Load(aiNode* assimpRootNode, aiMesh* assimpMesh) {
//    // Load bone names, store in bone map and cpy bone data.
//    for (unsigned int i = 0; i < assimpMesh->mNumBones; ++i) {
//        const aiBone* assimpBone = assimpMesh->mBones[i];
//        Bone& bone = this->boneMap[assimpMesh->mBones[i]->mName.data];
//
//        bone.name = assimpMesh->mBones[i]->mName.data;
//        CpyMat(assimpBone->mOffsetMatrix, bone.offsetMatrix);
//		bone.inverseOffsetMatrix = glm::inverse(bone.offsetMatrix);
//        bone.weightNr = assimpBone->mNumWeights;
//        bone.weightData = new Skeleton::Bone::VertexWeight[bone.weightNr];
//        for (unsigned int w = 0; w < bone.weightNr; ++w) {
//            const aiVertexWeight* assimpWeight = &assimpBone->mWeights[w];
//            Skeleton::Bone::VertexWeight& vertexWeight = bone.weightData[w];
//            vertexWeight.vID = assimpWeight->mVertexId;
//            vertexWeight.weight = assimpWeight->mWeight;
//        }
//    }
//    // Build Node tree
//    this->BuildSkeleton(assimpRootNode, &this->rootNode, nullptr);
//}
//
//void Model::Skeleton::BuildSkeleton(aiNode* assimpNode, Node* node, Node* parentNode) {
//    // Cpy node data
//    node->name = assimpNode->mName.data;
//    node->childrenNr = assimpNode->mNumChildren;
//    node->childrenData = new Node*[node->childrenNr];
//    node->parent = parentNode;
//    CpyMat(assimpNode->mTransformation, node->transformation);
//	node->inverseTransform = glm::inverse(node->transformation);
//    for (unsigned int i = 0; i < assimpNode->mNumChildren; ++i) {
//        Node* childNode = node->childrenData[i] = new Node();
//        BuildSkeleton(assimpNode->mChildren[i], childNode, node);
//    }
//}
//
//Model::Skeleton::Bone* Model::Skeleton::FindBone(const char* name) {
//    auto& it = boneMap.find(name);
//    return it != boneMap.end() ? &it->second : nullptr;
//}
//
//Model::Skeleton::Node* Model::Skeleton::FindNode(const char* name) {
//    return FindNode(&this->rootNode, name);
//}
//
//Model::Skeleton::Node* Model::Skeleton::FindNode(Node* node, const char* name) {
//    if (!std::strcmp(name, node->name.c_str()))
//        return node;
//    for (unsigned int i = 0; i < node->childrenNr; i++) {
//        Node* found = FindNode(node->childrenData[i], name);
//        if (found)
//            return found;
//    }
//    return nullptr;
//}
//
//void Model::Mesh::Clear() {
//    if (vertexData != nullptr) {
//        delete[] vertexData;
//        vertexData = nullptr;
//        vertexNr = 0;
//    }
//    if (indexData != nullptr) {
//        indexData = nullptr;
//        indexNr = 0;
//    }
//}
//
//void Model::Skeleton::Clear() {
//    Clear(&this->rootNode);
//    for (auto& it : this->boneMap) {
//        it.second.Clear();
//    }
//}
//
//void Model::Skeleton::Clear(Node* node) {
//    for (unsigned int i = 0; i < node->childrenNr; ++i)
//        Clear(node->childrenData[i]);
//    delete[] node->childrenData;
//    node->childrenData = nullptr;
//    node->childrenNr = 0;
//}
//
//
//void Model::Skeleton::Bone::Clear() {
//    if (weightData != nullptr) {
//        delete[] weightData;
//        weightData = nullptr;
//        weightNr = 0;
//    }
//}
//
//void Model::Animation::Clear() {
//    for (unsigned int i = 0; i < animChanelNr; ++i) {
//        Animation::AnimChanel* animChanel = &animChanelData[i];
//        delete[] animChanel->rotKeyData;
//        animChanel->rotKeyData = nullptr;
//        animChanel->rotKeyNr = 0;
//        delete[] animChanel->posKeyData;
//        animChanel->posKeyData = nullptr;
//        animChanel->posKeyNr = 0;
//        delete[] animChanel->scaleKeyData;
//        animChanel->scaleKeyData = nullptr;
//        animChanel->scaleKeyNr = 0;
//    }
//    delete[] animChanelData;
//    animChanelData = nullptr;
//    animChanelNr = 0;
//}

void Model::MixVec(const glm::vec3& v1, const glm::vec3& v2, float t, glm::vec3& result) {
    result.x = v1.x + t * (v2.x - v1.x);
    result.y = v1.y + t * (v2.y - v1.y);
    result.z = v1.z + t * (v2.z - v1.z);
}

void Model::MixQuat(const glm::quat& q1, const glm::quat& q2, float t, glm::quat& result) {
    glm::quat tmp = q1;
    if (DotQuat(q1, q2) < 0) {
        tmp.x = -q1.x;
        tmp.y = -q1.y;
        tmp.z = -q1.z;
        tmp.w = -q1.w;
    }
    result.x = tmp.x + t * (q2.x - tmp.x);
    result.y = tmp.y + t * (q2.y - tmp.y);
    result.z = tmp.z + t * (q2.z - tmp.z);
    result.w = tmp.w + t * (q2.w - tmp.w);
    NormalizeQuat(result);
}

float Model::DotQuat(const glm::quat& q1, const glm::quat& q2) {
    return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
}

void Model::NormalizeQuat(glm::quat& q) {
    float d = sqrt(DotQuat(q, q));
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

void Model::ComposeMatrix(const glm::vec3& p, glm::quat& r, const glm::vec3& s, glm::mat4& m) {
    // Rotation
    m[0][0] = 1.f - 2.f * (r.y * r.y + r.z * r.z);
    m[0][1] = 2.f * (r.x * r.y - r.z * r.w);
    m[0][2] = 2.f * (r.x * r.z + r.y * r.w);
    m[1][0] = 2.f * (r.x * r.y + r.z * r.w);
    m[1][1] = 1.f - 2.f * (r.x * r.x + r.z * r.z);
    m[1][2] = 2.f * (r.y * r.z - r.x * r.w);
    m[2][0] = 2.f * (r.x * r.z - r.y * r.w);
    m[2][1] = 2.f * (r.y * r.z + r.x * r.w);
    m[2][2] = 1.f - 2.f * (r.x * r.x + r.y * r.y);

    // Scale
    m[0][0] *= s.x;
    m[0][1] *= s.x;
    m[0][2] *= s.x;
    m[1][0] *= s.y;
    m[1][1] *= s.y;
    m[1][2] *= s.y;
    m[2][0] *= s.z;
    m[2][1] *= s.z;
    m[2][2] *= s.z;

    // Translation
    m[0][3] = p.x;
    m[1][3] = p.y;
    m[2][3] = p.z;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
}

aiNode* Model::FindNode(aiNode* node, const char* name) {
    if (!std::strcmp(name, node->mName.data))
        return node;
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        aiNode* found = FindNode(node->mChildren[i], name);
        if (found)
            return found;
    }
    return nullptr;
}
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

void Model::CpyVec(glm::vec3& glmVec, const aiVector3D& aiVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
    glmVec.z = aiVec.z;
}

void Model::CpyVec(glm::vec2& glmVec, const aiVector3D& aiVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
}


void Model::CpyVec(glm::vec2& glmVec, const aiVector2D& aiVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
}

void Model::CpyMat(glm::mat4& glmMat, const aiMatrix4x4& aiMat) {
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

void Model::CpyMat(aiMatrix3x3& aiMat3, const aiMatrix4x4& aiMat4) {
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

void Model::CpyQuat(glm::quat &glmQuat, const aiQuaternion& aiQuat) {
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
