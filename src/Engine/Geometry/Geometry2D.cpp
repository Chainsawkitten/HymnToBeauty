#include "Geometry2D.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

using namespace Geometry;

Geometry2D::~Geometry2D() {
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteBuffers(1, &indexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}

GLuint Geometry2D::VertexArray() const {
    return vertexArray;
}

void Geometry2D::GenerateBuffers() {
    // Vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, VertexCount() * sizeof(Vertex), Vertices(), GL_STATIC_DRAW);
    
    // Index buffer
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexCount() * sizeof(unsigned int), Indices(), GL_STATIC_DRAW);
}

void Geometry2D::GenerateVertexArray() {
    // Define vertex data layout
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), BUFFER_OFFSET(sizeof(float) * 2));
    
    glBindVertexArray(0);
}
