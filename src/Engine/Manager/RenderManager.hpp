#pragma once

#include <glm/glm.hpp>

class Scene;
class Shader;
class ShaderProgram;
class StaticRenderProgram;
class SkinRenderProgram;
class DeferredLighting;
class PostProcessing;
class FXAAFilter;
class GammaCorrectionFilter;
class GlowFilter;
class GlowBlurFilter;

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
        
        Shader* defaultVertexShader;
        Shader* defaultFragmentShader;
        Shader* skinningVertexShader;
        ShaderProgram* staticShaderProgram;
        ShaderProgram* skinShaderProgram;
        StaticRenderProgram* staticRenderProgram;
        SkinRenderProgram* skinRenderProgram;
        
        DeferredLighting* deferredLighting;
        
        // Post-processing.
        PostProcessing* postProcessing;
        FXAAFilter* fxaaFilter;
        GammaCorrectionFilter* gammaCorrectionFilter;
        GlowFilter* glowFilter;
        GlowBlurFilter* glowBlurFilter;
};
