#include "DebugDrawingManager.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

DebugDrawingManager::DebugDrawingManager() {
    // Create point vertex array.
    glBindVertexArray(0);
    
    // Vertex buffer
    glGenBuffers(1, &pointVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pointVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::vec3), &glm::vec3(0.f, 0.f, 0.f)[0], GL_STATIC_DRAW);
    
    // Define vertex data layout
    glGenVertexArrays(1, &pointVertexArray);
    glBindVertexArray(pointVertexArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, pointVertexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), BUFFER_OFFSET(0));
    
    glBindVertexArray(0);
}

DebugDrawingManager::~DebugDrawingManager() {
    glDeleteBuffers(1, &pointVertexBuffer);
    glDeleteVertexArrays(1, &pointVertexArray);
}

void DebugDrawingManager::AddPoint(const glm::vec3& position, const glm::vec3& color, float duration, bool depthTesting) {
    Point point;
    point.position = position;
    point.color = color;
    point.duration = duration;
    point.depthTesting = depthTesting;
    points.push_back(point);
}
