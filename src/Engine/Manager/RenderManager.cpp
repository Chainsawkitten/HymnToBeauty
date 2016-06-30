#include "RenderManager.hpp"

#include <GL/glew.h>
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "Default3D.vert.hpp"
#include "Default3D.frag.hpp"

RenderManager::RenderManager() {
    vertexShader = Managers().resourceManager->CreateShader(DEFAULT3D_VERT, DEFAULT3D_VERT_LENGTH, GL_VERTEX_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(DEFAULT3D_FRAG, DEFAULT3D_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, fragmentShader });
}

RenderManager::~RenderManager() {
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(fragmentShader);
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
}

void RenderManager::Render(Scene& scene, const glm::vec2& screenSize) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
}
