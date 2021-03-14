#include "Model.hpp"

#include <cstring>
#include "../Hymn.hpp"
#include <Utility/Log.hpp>
#include "MeshData.hpp"
#include <fstream>
#include <Video/Geometry/VertexType/StaticVertex.hpp>

using namespace Geometry;

Model::Model(Video::LowLevelRenderer* lowLevelRenderer) : Video::Geometry::Geometry3D(lowLevelRenderer) {}

Model::~Model() {}

Json::Value Model::Save() const {
    Json::Value model;
    model["name"] = name;
    return model;
}

void Model::Load(const std::string& name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);
    LoadGeometry((Hymn().GetPath() + "/" + name).c_str());
}

void Model::LoadGeometry(const char* filename) {
    // Read geometry data.
    std::ifstream file;
    file.open(filename, std::ios::binary);
    if (!file.is_open()) {
        Log(Log::ERR) << "Failed to load model " << filename << ". Could not open file.\n";
        return;
    }

    uint32_t vertexCount, indexCount;
    glm::vec3 min, max;

    file.read(reinterpret_cast<char*>(&vertexCount), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&indexCount), sizeof(uint32_t));
    file.read(reinterpret_cast<char*>(&min[0]), sizeof(float));
    file.read(reinterpret_cast<char*>(&min[1]), sizeof(float));
    file.read(reinterpret_cast<char*>(&min[2]), sizeof(float));
    file.read(reinterpret_cast<char*>(&max[0]), sizeof(float));
    file.read(reinterpret_cast<char*>(&max[1]), sizeof(float));
    file.read(reinterpret_cast<char*>(&max[2]), sizeof(float));

    Video::Geometry::VertexType::StaticVertex* vertices = new Video::Geometry::VertexType::StaticVertex[vertexCount];
    file.read(reinterpret_cast<char*>(vertices), sizeof(Video::Geometry::VertexType::StaticVertex) * vertexCount);

    uint32_t* indices = new uint32_t[indexCount];
    file.read(reinterpret_cast<char*>(indices), sizeof(uint32_t) * indexCount);

    file.close();

    // Create buffers.
    vertexBuffer = Video::Geometry::VertexType::StaticVertex::GenerateVertexBuffer(lowLevelRenderer, vertices, vertexCount);
    vertexDescription = Video::Geometry::VertexType::StaticVertex::GenerateVertexDescription(lowLevelRenderer);

    GenerateIndexBuffer(indices, indexCount);
    GenerateGeometryBinding();

    CreateAxisAlignedBoundingBox(max - min, (min + max) * 0.5f, min, max);

    // Clean up.
    delete[] vertices;
    delete[] indices;
}
