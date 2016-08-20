#pragma once

#include <glm/glm.hpp>

class Hub;
class Scene;
class Shader;
class ShaderProgram;
class DeferredLighting;
class PostProcessing;
class FXAAFilter;
class GammaCorrectionFilter;

/// Handles rendering a scene.
class RenderManager {
    friend class Hub;
    
    public:
        /// Render Scene containing entities.
        /**
         * @param scene Contains a bunch of entities.
         */
        void Render(Scene& scene);
    
    private:
        RenderManager();
        ~RenderManager();
        RenderManager(RenderManager const&) = delete;
        void operator=(RenderManager const&) = delete;
        
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
        
        DeferredLighting* deferredLighting;
        
        // Post-processing.
        PostProcessing* postProcessing;
        FXAAFilter* fxaaFilter;
        GammaCorrectionFilter* gammaCorrectionFilter;
};
