#pragma once

#include <glm/glm.hpp>

class Hub;
class Scene;
class Shader;
class ShaderProgram;

/// Handles rendering a scene.
class RenderManager {
    friend class Hub;
    
    public:
        /// Render Scene containing entities.
        /**
         * @param scene Contains a bunch of entities.
         * @param screenSize Size of the render target in pixels.
         */
        void Render(Scene& scene, const glm::vec2& screenSize);
    
    private:
        RenderManager(const glm::vec2& screenSize);
        ~RenderManager();
        RenderManager(RenderManager const&) = delete;
        void operator=(RenderManager const&) = delete;
        
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
};
