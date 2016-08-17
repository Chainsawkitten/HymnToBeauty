#include "Geometry3D.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

using namespace Geometry;

Geometry3D::~Geometry3D() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
}

GLuint Geometry3D::GetVertexArray() const {
    return vertexArray;
}

const Physics::AxisAlignedBoundingBox& Geometry3D::GetAxisAlignedBoundingBox() const {
    return axisAlignedBoundingBox;
}

void Geometry3D::GenerateBuffers() {
    glBindVertexArray(0);
    
    // Vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, GetVertexCount() * sizeof(Vertex), GetVertices(), GL_STATIC_DRAW);
    
    // Index buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndexCount() * sizeof(unsigned int), GetIndices(), GL_STATIC_DRAW);
}

void Geometry3D::GenerateVertexArray() {
    // Define vertex data layout
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry3D::Vertex), BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Geometry3D::Vertex), BUFFER_OFFSET(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry3D::Vertex), BUFFER_OFFSET(sizeof(float) * 5));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Geometry3D::Vertex), BUFFER_OFFSET(sizeof(float) * 8));
    
    glBindVertexArray(0);
}

void Geometry3D::CreateAxisAlignedBoundingBox() {
    glm::vec3 minValues, maxValues, origin, dim;
    Vertex* currVert = GetVertices();
    minValues = maxValues = origin = glm::vec3(0.f, 0.f, 0.f);
    unsigned int numVerts = GetVertexCount();
    
    // Find minimum/maximum bounding points.
    for (unsigned int i = 0; i < numVerts; i++) {
        if (currVert[i].position.x > maxValues.x)
            maxValues.x = currVert[i].position.x;
        else if (currVert[i].position.x < minValues.x)
            minValues.x = currVert[i].position.x;
        
        if (currVert[i].position.y > maxValues.y)
            maxValues.y = currVert[i].position.y;
        else if (currVert[i].position.y < minValues.y)
            minValues.y = currVert[i].position.y;
        
        if (currVert[i].position.z > maxValues.z)
            maxValues.z = currVert[i].position.z;
        else if (currVert[i].position.z < minValues.z)
            minValues.z = currVert[i].position.z;
    }
    
    // Set origin.
    origin.x = (minValues.x + maxValues.x) / 2.f;
    origin.y = (minValues.y + maxValues.y) / 2.f;
    origin.z = (minValues.z + maxValues.z) / 2.f;
    
    // Dimensions.
    dim.x = maxValues.x - minValues.x;
    dim.y = maxValues.y - minValues.y;
    dim.z = maxValues.z - minValues.z;
    axisAlignedBoundingBox = Physics::AxisAlignedBoundingBox(dim, origin, minValues, maxValues);
}
