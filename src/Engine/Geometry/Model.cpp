#include "Model.hpp"

#include <cstring>
#include "../Hymn.hpp"
#include <Utility/Log.hpp>
#include "MeshData.hpp"

using namespace Geometry;

Model::Model() {}

Model::~Model() {}

Json::Value Model::Save() const {
    Json::Value model;
    model["name"] = name;
    model["type"] = type == STATIC ? "Static" : "Skin";
    return model;
}

void Model::Load(const std::string& name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);
    Load((Hymn().GetPath() + "/" + name + ".asset").c_str());
}

void Model::Load(const char* filename) {
    if (assetFile.Open(filename, AssetFileHandler::READ)) {
        assetFile.LoadMeshData(0);
        MeshData* meshData = assetFile.GetStaticMeshData();
        type = meshData->isSkinned ? SKIN : STATIC;

        if (meshData->CPU) {
            vertexPositionData.resize(meshData->numVertices);
            if (meshData->isSkinned)
                for (std::size_t i = 0; i < meshData->numVertices; ++i)
                    vertexPositionData[i] = meshData->skinnedVertices[i].position;
            else
                for (std::size_t i = 0; i < meshData->numVertices; ++i)
                    vertexPositionData[i] = meshData->staticVertices[i].position;

            vertexIndexData.resize(meshData->numIndices);
            std::memcpy(vertexIndexData.data(), meshData->indices, sizeof(uint32_t) * meshData->numIndices);
        }

        if (meshData->GPU) {
            if (meshData->isSkinned) {
                GenerateVertexBuffer(vertexBuffer, meshData->skinnedVertices, meshData->numVertices);
                GenerateIndexBuffer(meshData->indices, meshData->numIndices, indexBuffer);
                GenerateSkinVertexArray(vertexBuffer, indexBuffer, vertexArray);
            } else {
                GenerateVertexBuffer(vertexBuffer, meshData->staticVertices, meshData->numVertices);
                GenerateIndexBuffer(meshData->indices, meshData->numIndices, indexBuffer);
                GenerateStaticVertexArray(vertexBuffer, indexBuffer, vertexArray);
            }
        }

        CreateAxisAlignedBoundingBox(meshData->aabbDim, meshData->aabbOrigin, meshData->aabbMinpos, meshData->aabbMaxpos);
        assetFile.Close();
    }
}

Model::Type Model::GetType() const {
    return type;
}

void Model::GenerateVertexBuffer(GLuint& vertexBuffer, Video::Geometry::VertexType::StaticVertex* vertices, unsigned int numVerticies) {
    vertexBuffer = Video::Geometry::VertexType::StaticVertex::GenerateVertexBuffer(vertices, numVerticies);
}

void Model::GenerateVertexBuffer(GLuint& vertexBuffer, Video::Geometry::VertexType::SkinVertex* vertices, unsigned int numVerticies) {
    vertexBuffer = Video::Geometry::VertexType::SkinVertex::GenerateVertexBuffer(vertices, numVerticies);
}

void Model::GenerateStaticVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray) {
    vertexArray = Video::Geometry::VertexType::StaticVertex::GenerateVertexArray(vertexBuffer, indexBuffer);
}

void Model::GenerateSkinVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray) {
    vertexArray = Video::Geometry::VertexType::SkinVertex::GenerateVertexArray(vertexBuffer, indexBuffer);
}
