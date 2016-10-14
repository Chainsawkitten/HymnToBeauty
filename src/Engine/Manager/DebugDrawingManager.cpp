#include "DebugDrawingManager.hpp"

#include "Managers.hpp"
#include "ResourceManager.hpp"
#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "DebugDrawing.vert.hpp"
#include "DebugDrawing.frag.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

DebugDrawingManager::DebugDrawingManager() {
    vertexShader = Managers().resourceManager->CreateShader(DEBUGDRAWING_VERT, DEBUGDRAWING_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(DEBUGDRAWING_FRAG, DEBUGDRAWING_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    
    // Create point vertex array.
    glBindVertexArray(0);
    
    glGenBuffers(1, &pointVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, pointVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(glm::vec3), &glm::vec3(0.f, 0.f, 0.f)[0], GL_STATIC_DRAW);
    
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
    
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
}

void DebugDrawingManager::AddPoint(const glm::vec3& position, const glm::vec3& color, float duration, bool depthTesting) {
    Point point;
    point.position = position;
    point.color = color;
    point.duration = duration;
    point.depthTesting = depthTesting;
    points.push_back(point);
}

void DebugDrawingManager::Render(Scene& scene, float deltaTime) {
    /// @todo Render debug entities.
}
