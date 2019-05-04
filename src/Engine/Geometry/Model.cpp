#include "Model.hpp"

#include <cstring>
#include "../Hymn.hpp"
#include <Utility/Log.hpp>
#include "MeshData.hpp"

using namespace Geometry;

Model::Model(Video::LowLevelRenderer* lowLevelRenderer) : Video::Geometry::Geometry3D(lowLevelRenderer) {}

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
    LoadGeometry((Hymn().GetPath() + "/" + name + ".asset").c_str());
}

Model::Type Model::GetType() const {
    return type;
}

void Model::LoadGeometry(const char* filename) {
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
                vertexBuffer = Video::Geometry::VertexType::SkinVertex::GenerateVertexBuffer(lowLevelRenderer, meshData->skinnedVertices, meshData->numVertices);
                vertexDescription = Video::Geometry::VertexType::SkinVertex::GenerateVertexDescription(lowLevelRenderer);
            } else {
                vertexBuffer = Video::Geometry::VertexType::StaticVertex::GenerateVertexBuffer(lowLevelRenderer, meshData->staticVertices, meshData->numVertices);
                vertexDescription = Video::Geometry::VertexType::StaticVertex::GenerateVertexDescription(lowLevelRenderer);
            }

            GenerateIndexBuffer(meshData->indices, meshData->numIndices);
            GenerateGeometryBinding();
        }

        CreateAxisAlignedBoundingBox(meshData->aabbDim, meshData->aabbOrigin, meshData->aabbMinpos, meshData->aabbMaxpos);
        assetFile.Close();
    }
}
