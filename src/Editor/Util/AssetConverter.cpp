#include "AssetConverter.hpp"
#include <Utility/Log.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Geometry/MeshData.hpp>

AssetConverter::AssetConverter() {
}

AssetConverter::~AssetConverter() {
}

void AssetConverter::Convert(const char* filepath, const char* destination, const glm::vec3& scale, bool triangulate, bool importNormals, bool importTangents, bool flipUVs, bool importMaterial, Material& materials, bool CPU, bool GPU) {
    success = true;
    errorString.clear();

    Geometry::AssetFileHandler file;

    // Return if file is not open.
    file.Open(destination, Geometry::AssetFileHandler::WRITE);

    unsigned int flags = triangulate ? aiProcess_Triangulate : 0;
    flags = importNormals ? flags : flags | aiProcess_CalcTangentSpace;
    flags = importTangents ? flags : flags | aiProcess_GenSmoothNormals;
    flags |= aiProcess_Debone;
    flags |= aiProcess_LimitBoneWeights;

    const aiScene* aScene = aImporter.ReadFile(filepath, flags);

    if (aScene == nullptr) {
        Log() << "Error importing mesh: " << filepath << "\n";
        Log() << aImporter.GetErrorString() << "\n";
    } else {
        if (importMaterial) {
            if (aScene->mMeshes[0]->mMaterialIndex >= 0) {
                aiMaterial* material = aScene->mMaterials[aScene->mMeshes[0]->mMaterialIndex];
                LoadMaterial(material, aiTextureType_DIFFUSE, materials.albedo);
                LoadMaterial(material, aiTextureType_NORMALS, materials.normal);
                LoadMaterial(material, aiTextureType_SPECULAR, materials.roughness);
                LoadMaterial(material, aiTextureType_REFLECTION, materials.metallic);
            }
        }
    
        ConvertMeshes(aScene, &file, scale, flipUVs, CPU, GPU);
    }

    aImporter.FreeScene();

    file.Close();
}

bool AssetConverter::Success() const {
    return success;
}

std::string& AssetConverter::GetErrorString() {
    return errorString;
}

void AssetConverter::ConvertMeshes(const aiScene* aScene, Geometry::AssetFileHandler* file, const glm::vec3& scale, bool flipUVs, bool CPU, bool GPU) {
    for (unsigned int i = 0; i < aScene->mNumMeshes; ++i)
        ConvertMesh(aScene->mMeshes[i], file, scale, flipUVs, CPU, GPU);
}

void AssetConverter::ConvertMesh(aiMesh* aMesh, Geometry::AssetFileHandler* file, const glm::vec3& scale, bool flipUVs, bool CPU, bool GPU) {
    Geometry::MeshData* meshData = new Geometry::MeshData;

    meshData->CPU = CPU;
    meshData->GPU = GPU;

    meshData->parent = 0;

    // Convert vertices.
    unsigned int numVertices = aMesh->mNumVertices;
    meshData->numVertices = numVertices;

    // If mesh has no bones the mesh is static.
    if (aMesh->mNumBones == 0) {
        meshData->staticVertices = ConvertStaticVertices(aMesh, file, numVertices, scale, flipUVs);
        meshData->isSkinned = false;
    }
    else {
        meshData->skinnedVertices = ConvertSkinnedVertices(aMesh, file, numVertices, scale, flipUVs);
        meshData->isSkinned = true;
    }

    // Convert indicies. 3 indicies for a face/triangle.
    unsigned int numIndicies = aMesh->mNumFaces * 3;
    uint32_t* indices = new uint32_t[numIndicies];
    unsigned int indexCounter = 0;
    for (unsigned int i = 0; i < aMesh->mNumFaces; ++i) {
        const aiFace& aFace = aMesh->mFaces[i];
        if (aFace.mNumIndices != 3) {
            errorString.append("ERROR: Mesh not triangulated.\n");
            success = false;
            delete[] indices;
            delete meshData;
            return;
        }

        indices[indexCounter++] = aFace.mIndices[0];
        indices[indexCounter++] = aFace.mIndices[1];
        indices[indexCounter++] = aFace.mIndices[2];
    }

    meshData->numIndices = indexCounter;
    meshData->indices = indices;
    
    CalculateAABB(meshData, meshData->numVertices);

    file->SaveMesh(meshData);

    delete meshData;
    meshData = nullptr;
}

Video::Geometry::VertexType::StaticVertex* AssetConverter::ConvertStaticVertices(aiMesh* aMesh, Geometry::AssetFileHandler* file, unsigned int numVertices, const glm::vec3& scale, bool flipUVs) {
    Video::Geometry::VertexType::StaticVertex* vertices = new Video::Geometry::VertexType::StaticVertex[numVertices];

    // Positions.
    if (aMesh->HasNormals()) {
        for (unsigned int i = 0; i < numVertices; ++i) {
            Geometry::CpyVec(vertices[i].position, aMesh->mVertices[i]);
            vertices[i].position *= scale;
        }
    } else {
        success = false;
        errorString.append("WARNING: The model has no positions yet the user is trying to import them.\n");
        for (unsigned int i = 0; i < numVertices; ++i)
            vertices[i].position = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    // Texture coordinates.
    if (aMesh->HasTextureCoords(0)) {
        for (unsigned int i = 0; i < numVertices; ++i) {
            Geometry::CpyVec(vertices[i].textureCoordinate, aMesh->mTextureCoords[0][i]);
            if (flipUVs) 
                vertices[i].textureCoordinate.y = 1.0f - vertices[i].textureCoordinate.y;
        }
    } else {
        success = false;
        errorString.append("WARNING: The model has no texture coordinates yet the user is trying to import them.\n");
        for (unsigned int i = 0; i < numVertices; ++i)
            vertices[i].textureCoordinate = glm::vec2(0.0f, 0.0f);
    }

    // Normals.
    if (aMesh->HasNormals()) {
        for (unsigned int i = 0; i < numVertices; ++i) {
            Geometry::CpyVec(vertices[i].normal, aMesh->mNormals[i]);
        }
    } else {
        success = false;
        errorString.append("WARNING: The model has no normals yet the user is trying to import them.\n");
        for (unsigned int i = 0; i < numVertices; ++i)
            vertices[i].normal = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    // Tangents.
    if (aMesh->HasTangentsAndBitangents()) {
        for (unsigned int i = 0; i < numVertices; ++i)
            Geometry::CpyVec(vertices[i].tangent, aMesh->mTangents[i]);
    } else {
        success = false;
        errorString.append("WARNING: The model has no tangents yet the user is trying to import them.\n");
        for (unsigned int i = 0; i < numVertices; ++i)
            vertices[i].tangent = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    return vertices;
}

Video::Geometry::VertexType::SkinVertex* AssetConverter::ConvertSkinnedVertices(aiMesh* aMesh, Geometry::AssetFileHandler* file, unsigned int numVertices, const glm::vec3& scale, bool flipUVs) {
    Video::Geometry::VertexType::SkinVertex* vertices = new Video::Geometry::VertexType::SkinVertex[numVertices];

    // Positions.
    if (aMesh->HasNormals()) {
        for (unsigned int i = 0; i < numVertices; ++i) {
            Geometry::CpyVec(vertices[i].position, aMesh->mVertices[i]);
            vertices[i].position *= scale;
        }
    } else {
        success = false;
        errorString.append("WARNING: The model has no positions yet the user is trying to import them.\n");
        for (unsigned int i = 0; i < numVertices; ++i)
            vertices[i].position = glm::vec3(1.f, 0.f, 0.f);
    }

    // Texture coordinates.
    if (aMesh->HasTextureCoords(0)) {
        for (unsigned int i = 0; i < numVertices; ++i) {
            Geometry::CpyVec(vertices[i].textureCoordinate, aMesh->mTextureCoords[0][i]);
            if (flipUVs)
                vertices[i].textureCoordinate.y = 1.0f - vertices[i].textureCoordinate.y;
        }
    } else {
        success = false;
        errorString.append("WARNING: The model has no texture coordinates yet the user is trying to import them.\n");
        for (unsigned int i = 0; i < numVertices; ++i)
            vertices[i].textureCoordinate = glm::vec2(0.0f, 0.0f);
    }

    // Normals.
    if (aMesh->HasNormals()) {
        for (unsigned int i = 0; i < numVertices; ++i)
            Geometry::CpyVec(vertices[i].normal, aMesh->mNormals[i]);
    } else {
        success = false;
        errorString.append("WARNING: The model has no normals yet the user is trying to import them.\n");
        for (unsigned int i = 0; i < numVertices; ++i)
            vertices[i].normal = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    // Tangents.
    if (aMesh->HasTangentsAndBitangents()) {
        for (unsigned int i = 0; i < numVertices; ++i)
            Geometry::CpyVec(vertices[i].tangent, aMesh->mTangents[i]);
    } else {
        success = false;
        errorString.append("WARNING: The model has no tangents yet the user is trying to import them.\n");
        for (unsigned int i = 0; i < numVertices; ++i)
            vertices[i].tangent = glm::vec3(1.0f, 0.0f, 0.0f);
    }

    std::vector<unsigned int> weightCounter(numVertices, 0);

    for (unsigned int b = 0; b < aMesh->mNumBones; ++b) {
        const aiBone* aBone = aMesh->mBones[b];
        for (unsigned int i = 0; i < aBone->mNumWeights; ++i) {
            unsigned int vertexID = aBone->mWeights[i].mVertexId;
            unsigned int& count = weightCounter[vertexID];
            vertices[vertexID].weights[count] = aBone->mWeights[i].mWeight;
            vertices[vertexID].boneIDs[count] = b;
            ++count;
        }
    }

    weightCounter.clear();

    return vertices;
}

void AssetConverter::CalculateAABB(Geometry::MeshData* meshData, unsigned int numVertices) {
    glm::vec3 minValues, maxValues, origin, dim;
    minValues = maxValues = origin = glm::vec3(0.f, 0.f, 0.f);

    // Find minimum/maximum bounding points.
    for (std::size_t i = 0; i < numVertices; ++i) {
        const glm::vec3& pos = meshData->isSkinned ? meshData->skinnedVertices[i].position : meshData->staticVertices[i].position;
        if (pos.x > maxValues.x)
            maxValues.x = pos.x;
        else if (pos.x < minValues.x)
            minValues.x = pos.x;

        if (pos.y > maxValues.y)
            maxValues.y = pos.y;
        else if (pos.y < minValues.y)
            minValues.y = pos.y;

        if (pos.z > maxValues.z)
            maxValues.z = pos.z;
        else if (pos.z < minValues.z)
            minValues.z = pos.z;
    }

    // Set origin.
    origin.x = (minValues.x + maxValues.x) / 2.f;
    origin.y = (minValues.y + maxValues.y) / 2.f;
    origin.z = (minValues.z + maxValues.z) / 2.f;

    // Dimensions.
    dim.x = maxValues.x - minValues.x;
    dim.y = maxValues.y - minValues.y;
    dim.z = maxValues.z - minValues.z;

    meshData->aabbDim = dim;
    meshData->aabbOrigin = origin;
    meshData->aabbMaxpos = maxValues;
    meshData->aabbMinpos = minValues;
}

void AssetConverter::LoadMaterial(aiMaterial* material, aiTextureType type, std::string& path) {
    if (material->GetTextureCount(type) > 0) {
        aiString p;
        material->GetTexture(type, 0, &p);
        path = p.C_Str();
    }
}
