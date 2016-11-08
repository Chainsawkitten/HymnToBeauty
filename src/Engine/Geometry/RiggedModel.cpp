#include "RiggedModel.hpp"

#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include "MathFunctions.hpp"
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
        aiProcess_FindInvalidData | \
        aiProcess_ValidateDataStructure | \
        0);

    Log() << aImporter.GetErrorString() << "\n";

    assert(aScene != nullptr);

    // Load skeleton.
    skeleton.Load(aScene);
        
    // Load meshes.
    LoadMeshes(aScene);

    // Load animation data.
    LoadAnimations(aScene);
    
    // Set model to bind pose.
    skeleton.BindPose();

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

    std::vector<unsigned int> weightCounter(numVertices, 0);
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
            ++numVertices;
        }
        // Load indices.
        for (unsigned int i = 0; i < aMesh->mNumFaces; ++i) {
            const aiFace& aFace = aMesh->mFaces[i];
            assert(aFace.mNumIndices == 3);
            indices[numIndices++] = entries[m].baseVertex + aFace.mIndices[0];
            indices[numIndices++] = entries[m].baseVertex + aFace.mIndices[1];
            indices[numIndices++] = entries[m].baseVertex + aFace.mIndices[2];
        }
        // Load Weights.
        assert(skeleton.GetNumBones() != 0); // Check if skeleton is loaded.
        for (unsigned int b = 0; b < aMesh->mNumBones; ++b) {
            const aiBone* aBone = aMesh->mBones[b];
            const size_t boneIndex = skeleton.FindBoneIndex(aBone->mName.data);
            assert(boneIndex != -1);
            for (unsigned int i = 0; i < aBone->mNumWeights; ++i) {
                unsigned int vertexID = entries[m].baseVertex + aBone->mWeights[i].mVertexId;
                float weight = aBone->mWeights[i].mWeight;
                unsigned int& count = weightCounter[vertexID];
                assert(count + 1 <= 4);
                vertices[vertexID].boneIDs[count] = boneIndex;
                vertices[vertexID].weights[count] = weight;
                ++count;
            }
        }
    }
}

void RiggedModel::LoadAnimations(const aiScene* aScene) {
    animations.resize(aScene->mNumAnimations);

    for (std::size_t a = 0; a < animations.size(); ++a) {
        animations[a].Load(aScene->mAnimations[a]);
    }
}

void RiggedModel::MeshTransform(const std::vector<glm::mat4>& transforms) {
    // CPU-skinning.
    unsigned int boneIDs[4];
    float boneWeights[4];

    // Vertex shader
    for (unsigned int v = 0; v < vertices.size(); ++v) {
        for (unsigned int i = 0; i < 4; ++i) {
            boneIDs[i] = vertices[v].boneIDs[i];
            boneWeights[i] = vertices[v].weights[i];
        }
        
        assert(abs(1.f - boneWeights[0] + boneWeights[1] + boneWeights[2] + boneWeights[3]) < 0.01f); // Assert weights sum equals 1.

        glm::mat4 boneTransform = transforms[boneIDs[0]] * boneWeights[0];
        boneTransform += transforms[boneIDs[1]] * boneWeights[1];
        boneTransform += transforms[boneIDs[2]] * boneWeights[2];
        boneTransform += transforms[boneIDs[3]] * boneWeights[3];

        VertexType::SkinVertex& vert = vertices[v];
        vert.position = boneTransform * glm::vec4(vert.position, 1.f);
        vert.normal = boneTransform * glm::vec4(vert.normal, 0.f);
    }
}
