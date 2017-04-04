#include "StaticVertex.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

const GLuint Geometry::VertexType::StaticVertex::GenerateVertexBuffer(StaticVertex* vertices, unsigned int vertexCount) {
    GLuint vertexBuffer;
    glBindVertexArray(0);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(StaticVertex), vertices, GL_STATIC_DRAW);
    return vertexBuffer;
}

const GLuint Geometry::VertexType::StaticVertex::GenerateVertexArray(GLuint vertexBuffer, GLuint indexBuffer) {
    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    
    const std::size_t stride = sizeof(StaticVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 5));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 8));
    
    glBindVertexArray(0);
    return vertexArray;
}
