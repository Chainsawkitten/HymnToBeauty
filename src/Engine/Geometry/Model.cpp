#include "Model.hpp"

#include "../Util/Log.hpp"
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"

using namespace Geometry;

Assimp::Importer Model::aiImporter = Assimp::Importer();

Model::Model() {

}

Model::Model(const char* filename) {
    Load(filename);
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

    const aiScene* scene = aiImporter.ReadFile(filename, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace );

    const aiMesh* mesh = scene->mMeshes[0];

    // Load Mesh
    vertexNr = mesh->mNumVertices;
    vertexData = new Vertex[vertexNr];
    for (unsigned int i = 0; i < vertexNr; ++i) {
        CpyVec(mesh->mVertices[i], vertexData[i].position);
        CpyVec(mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : aiVector3D(0.f, 0.f, 0.f), vertexData[i].textureCoordinate);
        CpyVec(mesh->HasNormals() ? mesh->mNormals[i] : aiVector3D(0.f, 0.f, 0.f), vertexData[i].normal);
        CpyVec(mesh->HasTangentsAndBitangents() ? mesh->mTangents[i] : aiVector3D(0.f, 0.f, 0.f), vertexData[i].tangent);
    }

    
    //struct Skeleton {

    //} skeleton;
    //// Load Skeleton
    //if (mesh->HasBones()) {
    //    unsigned int boneNr = mesh->mNumBones;
    //    aiMatrix4x4* boneData = new aiMatrix4x4[boneNr];
    //    for (unsigned int b = 0; b < boneNr; ++b) {
    //        boneData[b] = mesh->mBones[b]->mOffsetMatrix;
    //        mesh->mBones[b]
    //    }

    //    //aiVertexWeight** weightData = new aiVertexWeight*[boneNr];
    //    //for (unsigned int b = 0; b < boneNr; ++b) {
    //    //    unsigned int weightNr = mesh->mBones[b]->mNumWeights;
    //    //    weightData[b] = new aiVertexWeight[weightNr];
    //    //    boneData[b] = mesh->mBones[b]->mOffsetMatrix;
    //    //    mesh->mBones[b]->mNumWeights
    //    //}
    //}

    // TODO use faces?
    indexNr = vertexNr;
    indexData = new unsigned int[indexNr];
    for (unsigned int i = 0; i < indexNr; i++)
        indexData[i] = i;

    GenerateBuffers();
    GenerateVertexArray();
    //CreateAxisAlignedBoundingBox();

    delete[] vertexData;
    delete[] indexData;
}

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
    for (unsigned int y = 0; y < 4; ++y)
        for (unsigned int x = 0; x < 4; ++x)
            glmMat[x][y] = aiMat[x][y];
}
