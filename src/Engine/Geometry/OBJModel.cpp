#include "OBJModel.hpp"

#include <fstream>
#include "../Util/Log.hpp"
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"

using namespace Geometry;

OBJModel::OBJModel() {
    
}

OBJModel::OBJModel(const char* filename) {
    Load(filename);
}

Geometry3D::Vertex* OBJModel::GetVertices() const {
    return vertexData;
}

unsigned int OBJModel::GetVertexCount() const {
    return vertexNr;
}

unsigned int* OBJModel::GetIndices() const {
    return indexData;
}

unsigned int OBJModel::GetIndexCount() const {
    return indexNr;
}

Json::Value OBJModel::Save() const {
    Json::Value model;
    model["name"] = name;
    return model;
}

void OBJModel::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    Load((Hymn().GetPath() + FileSystem::DELIMITER + "Models" + FileSystem::DELIMITER + name + ".obj").c_str());
}

void OBJModel::Load(const char* filename) {
    std::ifstream modelFile;
    modelFile.open(filename);
    if (!modelFile.is_open())
        Log() << "Couldn't open model file " << filename << " for reading.\n";
    
    std::vector<Vertex> vertices;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec3> tangents;
    std::vector<glm::vec2> textureCoordinates;
    std::vector<Face> faces;
    
    // Parse OBJ file.
    while (!modelFile.eof()) {
        char c, c2;
        modelFile >> c;
        switch (c) {
        case 'v':
            modelFile >> std::noskipws >> c2 >> std::skipws;
            switch (c2) {
            case ' ':
                // Position
                positions.push_back(ReadVec3(modelFile));
                tangents.push_back(glm::vec3(0.f, 0.f, 0.f));
                break;
            case 'n':
                // Normal
                normals.push_back(glm::normalize(ReadVec3(modelFile)));
                break;
            case 't':
                // Texture coordinate
                textureCoordinates.push_back(ReadVec2(modelFile));
                break;
            }
            break;
        case 'f':
            // Face
            faces.push_back(ReadFace(modelFile));
            break;
        default:
            modelFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
    
    modelFile.close();
    
    CalculateTangents(faces, positions, textureCoordinates, tangents);
    
    for (const Face& face : faces) {
        for (int i = 0; i < 3; i++) {
            Vertex vertex;
            
            vertex.position = positions[face.vertices[i].positionIndex];
            vertex.textureCoordinate = textureCoordinates[face.vertices[i].textureIndex];
            
            // OpenGL has origin in lower left corner, not upper left corner.
            vertex.textureCoordinate.y = 1.f - vertex.textureCoordinate.y;
            
            vertex.normal = normals[face.vertices[i].normalIndex];
            vertex.tangent = tangents[face.vertices[i].positionIndex];
            
            vertices.push_back(vertex);
        }
    }
    
    vertexNr = vertices.size();
    vertexData = new Vertex[vertexNr];
    for (unsigned int i = 0; i < vertexNr; i++)
        vertexData[i] = vertices[i];
    
    indexNr = vertexNr;
    indexData = new unsigned int[indexNr];
    for (unsigned int i = 0; i < indexNr; i++)
        indexData[i] = i;
    
    GenerateBuffers();
    GenerateVertexArray();
    
    delete[] vertexData;
    delete[] indexData;
    
    CreateAxisAlignedBoundingBox();
}

glm::vec3 OBJModel::ReadVec3(std::ifstream& modelFile) {
    glm::vec3 vec;
    modelFile >> vec.x >> vec.y >> vec.z;
    return vec;
}

glm::vec2 OBJModel::ReadVec2(std::ifstream& modelFile) {
    glm::vec2 vec;
    modelFile >> vec.x >> vec.y;
    return vec;
}

OBJModel::Face OBJModel::ReadFace(std::ifstream& modelFile) {
    Face face;
    
    for (int i = 0; i < 3; i++)
        face.vertices[i] = ReadVertex(modelFile);
    
    return face;
}

OBJModel::Face::Vertex OBJModel::ReadVertex(std::ifstream& modelFile) {
    Face::Vertex vertex;
    char delimiter;
    unsigned int index;
    
    // Position.
    modelFile >> index;
    vertex.positionIndex = index - 1;
    
    // Texture coordinate.
    modelFile >> delimiter >> index;
    vertex.textureIndex = index - 1;
    
    // Normal.
    modelFile >> delimiter >> index;
    vertex.normalIndex = index - 1;
    
    return vertex;
}

void OBJModel::CalculateTangents(std::vector<Face>& faces, std::vector<glm::vec3>& positions, std::vector<glm::vec2>& textureCoordinates, std::vector<glm::vec3>& tangents) {
    for (const Face& face : faces) {
        glm::vec3 edge1 = positions[face.vertices[1].positionIndex] - positions[face.vertices[0].positionIndex];
        glm::vec3 edge2 = positions[face.vertices[2].positionIndex] - positions[face.vertices[0].positionIndex];
        
        glm::vec2 deltaUV1 = textureCoordinates[face.vertices[1].textureIndex] - textureCoordinates[face.vertices[0].textureIndex];
        glm::vec2 deltaUV2 = textureCoordinates[face.vertices[2].textureIndex] - textureCoordinates[face.vertices[0].textureIndex];
        
        float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        
        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        
        tangents[face.vertices[0].positionIndex] += tangent;
        tangents[face.vertices[1].positionIndex] += tangent;
        tangents[face.vertices[2].positionIndex] += tangent;
    }
    
    for (glm::vec3& tangent : tangents) {
        tangent = glm::normalize(tangent);
    }
}
