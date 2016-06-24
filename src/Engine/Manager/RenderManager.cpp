#include "RenderManager.hpp"

#include <GL/glew.h>

RenderManager::RenderManager() {
    
}

RenderManager::~RenderManager() {
    
}

void RenderManager::Render(Scene& scene) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
