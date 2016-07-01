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
