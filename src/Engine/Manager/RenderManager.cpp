#include "RenderManager.hpp"

#include <GL/glew.h>

RenderManager::RenderManager() {
    
}

RenderManager::~RenderManager() {
    
}

void RenderManager::Render(Scene& scene, const glm::vec2& screenSize) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, static_cast<GLsizei>(screenSize.x), static_cast<GLsizei>(screenSize.y));
}
