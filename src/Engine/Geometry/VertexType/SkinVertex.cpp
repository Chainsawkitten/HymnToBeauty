#include "SkinVertex.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

const GLuint Geometry::VertexType::SkinVertex::GenerateVertexBuffer(SkinVertex* vertices, unsigned int vertexCount) {
    GLuint vertexBuffer;
    glBindVertexArray(0);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(SkinVertex), vertices, GL_STATIC_DRAW);
    return vertexBuffer;
}

const GLuint Geometry::VertexType::SkinVertex::GenerateVertexArray(GLuint vertexBuffer, GLuint indexBuffer) {
    GLuint vertexArray;
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    
    const std::size_t stride = sizeof(SkinVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 5));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 8));
    glVertexAttribIPointer(4, 4, GL_INT, stride, BUFFER_OFFSET(sizeof(float) * 11));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(float) * 11 + sizeof(int) * 4));
    
    glBindVertexArray(0);
    return vertexArray;
}
