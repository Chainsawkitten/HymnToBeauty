#include "Cube.hpp"

using namespace Geometry;

Cube::Cube() {
    // Vertices
    vertexCount = 24;
    vertexData = new Vertex[vertexCount];
    
    // Side 1
    vertexData[0] = {
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec2(1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    vertexData[1] = {
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec2(0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    vertexData[2] = {
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec2(1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    vertexData[3] = {
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec2(0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    
    // Side 2
    vertexData[4] = {
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec2(0.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
    vertexData[5] = {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec2(0.0f, 1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
    vertexData[6] = {
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec2(1.0f, 1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
    vertexData[7] = {
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec2(1.0f, 0.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    };
    
    // Side 3
    vertexData[8] = {
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec2(1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    vertexData[9] = {
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec2(1.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    vertexData[10] = {
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec2(0.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    vertexData[11] = {
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec2(0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    
    // Side 4
    vertexData[12] = {
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec2(0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f)
    };
    vertexData[13] = {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec2(1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f)
    };
    vertexData[14] = {
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec2(0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f)
    };
    vertexData[15] = {
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec2(1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3(-1.0f, 0.0f, 0.0f)
    };
    
    // Side 5
    vertexData[16] = {
        glm::vec3(-0.5f, -0.5f, 0.5f),
        glm::vec2(0.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    vertexData[17] = {
        glm::vec3(0.5f, -0.5f, 0.5f),
        glm::vec2(1.0f, 0.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    vertexData[18] = {
        glm::vec3(0.5f, -0.5f, -0.5f),
        glm::vec2(1.0f, 1.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    vertexData[19] = {
        glm::vec3(-0.5f, -0.5f, -0.5f),
        glm::vec2(0.0f, 1.0f),
        glm::vec3(0.0f, -1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    
    // Side 6
    vertexData[20] = {
        glm::vec3(-0.5f, 0.5f, 0.5f),
        glm::vec2(0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    vertexData[21] = {
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec2(0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    vertexData[22] = {
        glm::vec3(0.5f, 0.5f, -0.5f),
        glm::vec2(1.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    vertexData[23] = {
        glm::vec3(-0.5f, 0.5f, -0.5f),
        glm::vec2(1.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -1.0f)
    };
    
    // Vertexindices
    indexCount = 36;
    indexData = new unsigned int[indexCount];
    
    // Side 1
    indexData[0] = 0;
    indexData[1] = 1;
    indexData[2] = 2;
    indexData[3] = 0;
    indexData[4] = 3;
    indexData[5] = 1;
    
    // Side 2
    indexData[6] = 4;
    indexData[7] = 6;
    indexData[8] = 7;
    indexData[9] = 4;
    indexData[10] = 5;
    indexData[11] = 6;
    
    // Side 3
    indexData[12] = 11;
    indexData[13] = 9;
    indexData[14] = 8;
    indexData[15] = 11;
    indexData[16] = 10;
    indexData[17] = 9;
    
    // Side 4
    indexData[18] = 12;
    indexData[19] = 13;
    indexData[20] = 15;
    indexData[21] = 12;
    indexData[22] = 14;
    indexData[23] = 13;
    
    // Side 5
    indexData[24] = 16;
    indexData[25] = 19;
    indexData[26] = 18;
    indexData[27] = 16;
    indexData[28] = 18;
    indexData[29] = 17;
    
    // Side 6
    indexData[30] = 20;
    indexData[31] = 22;
    indexData[32] = 23;
    indexData[33] = 20;
    indexData[34] = 21;
    indexData[35] = 22;
    
    GenerateBuffers();
    GenerateVertexArray();
    CreateAxisAlignedBoundingBox();
}

Cube::~Cube() {
    delete[] vertexData;
    delete[] indexData;
}

Geometry3D::Vertex* Cube::GetVertices() const {
    return vertexData;
}

unsigned int Cube::GetVertexCount() const {
    return vertexCount;
}

unsigned int* Cube::GetIndices() const {
    return indexData;
}

unsigned int Cube::GetIndexCount() const {
    return indexCount;
}
