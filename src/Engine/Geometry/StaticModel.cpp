#include "StaticModel.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "MathFunctions.hpp"
#include "../Util/Log.hpp"

using namespace Geometry;

StaticModel::StaticModel() {

}

StaticModel::StaticModel(const char* filename) {
    Load(filename);
}

StaticModel::~StaticModel() {
}

void StaticModel::Load(const char* filename) {
    // Load assimp scene data. //TODO FIX FLAGS FOR STATIC MODEL
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

    // Load meshes.
    LoadMeshes(aScene);

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

const Geometry3D::Type StaticModel::GetType() const {
    return STATIC;
}

void StaticModel::GenerateVertexBuffer(GLuint& vertexBuffer) {
    vertexBuffer = VertexType::StaticVertex::GenerateVertexBuffer(vertices.data(), vertices.size());
}

void StaticModel::GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray) {
    vertexArray = VertexType::StaticVertex::GenerateVertexArray(vertexBuffer, indexBuffer);
}

void StaticModel::LoadMeshes(const aiScene* aScene) {
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

    numVertices = 0;
    numIndices = 0;

    // Initialize the meshes in the scene one by one.
    for (unsigned int m = 0; m < aScene->mNumMeshes; ++m) {
        const aiMesh* aMesh = aScene->mMeshes[m];
        // Load vertices.
        for (unsigned int i = 0; i < aMesh->mNumVertices; ++i) {
            VertexType::StaticVertex& vert = vertices[numVertices];
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
    }
}
