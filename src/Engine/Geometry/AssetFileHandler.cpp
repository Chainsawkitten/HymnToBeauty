#include "AssetFileHandler.hpp"
#include <Utility/Log.hpp>
#include "../Geometry/MeshData.hpp"

using namespace Geometry;

AssetFileHandler::AssetFileHandler() {
}

AssetFileHandler::AssetFileHandler(const char* filepath, Mode mode) {
    Open(filepath, mode);
}

AssetFileHandler::~AssetFileHandler() {
    Close();
}

bool AssetFileHandler::Open(const char* filepath, Mode mode) {
    // Close the file is already open.
    Close();

    this->mode = mode;

    if (mode == READ) {
        // Open the .asset file
        rFile.open(filepath, std::ios::binary);

        // Return false if file is not open.
        if (!rFile.is_open())
            return false;

        // Read version.
        rFile.read(reinterpret_cast<char*>(&fileVersion), sizeof(uint16_t));

        ReadGlobalHeader();
    } else {
        // Create the .asset file.
        wFile.open(filepath, std::ios::binary);
        if (!wFile.is_open()) {
            Log() << "Could not create file: " << filepath << "\n";
            return false;
        }

        // Write version.
        uint16_t verison = CURRENT_VERSION;
        wFile.write(reinterpret_cast<char*>(&verison), sizeof(uint16_t));

        uniqueID = 1;
        numStaticMeshes = 1;
        WriteGlobalHeader();
    }

    return true;
}

void AssetFileHandler::Close() {
    if (rFile.is_open())
        rFile.close();

    if (wFile.is_open())
        wFile.close();

    Clear();
}

void AssetFileHandler::Clear() {
    ClearMesh();
}

void AssetFileHandler::LoadMeshData(int meshID) {
    ClearMesh();

    meshData = new MeshData();

    rFile.read(reinterpret_cast<char*>(&meshData->parent), sizeof(uint32_t));
    rFile.read(reinterpret_cast<char*>(&meshData->numVertices), sizeof(uint32_t));
    rFile.read(reinterpret_cast<char*>(&meshData->numIndices), sizeof(uint32_t));

    rFile.read(reinterpret_cast<char*>(&meshData->aabbDim), sizeof(glm::vec3));
    rFile.read(reinterpret_cast<char*>(&meshData->aabbOrigin), sizeof(glm::vec3));
    rFile.read(reinterpret_cast<char*>(&meshData->aabbMinpos), sizeof(glm::vec3));
    rFile.read(reinterpret_cast<char*>(&meshData->aabbMaxpos), sizeof(glm::vec3));

    rFile.read(reinterpret_cast<char*>(&meshData->isSkinned), sizeof(bool));
    rFile.read(reinterpret_cast<char*>(&meshData->CPU), sizeof(bool));
    rFile.read(reinterpret_cast<char*>(&meshData->GPU), sizeof(bool));

    if (meshData->isSkinned) {
        meshData->skinnedVertices = new Video::Geometry::VertexType::SkinVertex[meshData->numVertices];
        rFile.read(reinterpret_cast<char*>(meshData->skinnedVertices),
            sizeof(Video::Geometry::VertexType::SkinVertex) * meshData->numVertices);
        meshData->staticVertices = nullptr;
    } else {
        meshData->staticVertices = new Video::Geometry::VertexType::StaticVertex[meshData->numVertices];
        rFile.read(reinterpret_cast<char*>(meshData->staticVertices),
            sizeof(Video::Geometry::VertexType::StaticVertex) * meshData->numVertices);
        meshData->skinnedVertices = nullptr;
    }

    meshData->indices = new uint32_t[meshData->numIndices];
    rFile.read(reinterpret_cast<char*>(meshData->indices), sizeof(uint32_t) * meshData->numIndices);
}

MeshData* AssetFileHandler::GetStaticMeshData() {
    return meshData;
}

void AssetFileHandler::SaveMesh(MeshData* meshData) {
    // Write header.
    wFile.write(reinterpret_cast<char*>(&meshData->parent), sizeof(uint32_t));
    wFile.write(reinterpret_cast<char*>(&meshData->numVertices), sizeof(uint32_t));
    wFile.write(reinterpret_cast<char*>(&meshData->numIndices), sizeof(uint32_t));

    // Write AABB data.
    wFile.write(reinterpret_cast<char*>(&meshData->aabbDim), sizeof(glm::vec3));
    wFile.write(reinterpret_cast<char*>(&meshData->aabbOrigin), sizeof(glm::vec3));
    wFile.write(reinterpret_cast<char*>(&meshData->aabbMinpos), sizeof(glm::vec3));
    wFile.write(reinterpret_cast<char*>(&meshData->aabbMaxpos), sizeof(glm::vec3));

    wFile.write(reinterpret_cast<char*>(&meshData->isSkinned), sizeof(bool));
    wFile.write(reinterpret_cast<char*>(&meshData->CPU), sizeof(bool));
    wFile.write(reinterpret_cast<char*>(&meshData->GPU), sizeof(bool));

    // Write mesh data.
    if (meshData->isSkinned) {
        wFile.write(reinterpret_cast<char*>(meshData->skinnedVertices),
            sizeof(Video::Geometry::VertexType::SkinVertex) * meshData->numVertices);
    } else {
        wFile.write(reinterpret_cast<char*>(meshData->staticVertices),
            sizeof(Video::Geometry::VertexType::StaticVertex) * meshData->numVertices);
    }

    wFile.write(reinterpret_cast<char*>(meshData->indices),
        sizeof(uint32_t) * meshData->numIndices);
}

void AssetFileHandler::ReadGlobalHeader() {
    rFile.read(reinterpret_cast<char*>(&uniqueID), sizeof(uint16_t));
    rFile.read(reinterpret_cast<char*>(&numStaticMeshes), sizeof(uint16_t));
}

void AssetFileHandler::WriteGlobalHeader() {
    wFile.write(reinterpret_cast<char*>(&uniqueID), sizeof(uint16_t));
    wFile.write(reinterpret_cast<char*>(&numStaticMeshes), sizeof(uint16_t));
}

void AssetFileHandler::ClearMesh() {
    if (meshData != nullptr) {
        delete meshData;
    }

    meshData = nullptr;
}
