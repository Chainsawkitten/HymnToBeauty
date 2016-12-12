#pragma once

#include <GL/glew.h>
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
        /// Render scene containing entities.
        /**
         * @param scene Contains a bunch of entities.
         */
        void Render(Scene& scene);
        
        /// Render editor entities.
        /**
         * @param scene Scene to render.
         */
        void RenderEditorEntities(Scene& scene);
    
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
        
        Shader* editorEntityVertexShader;
        Shader* editorEntityGeometryShader;
        Shader* editorEntityFragmentShader;
        ShaderProgram* editorEntityShaderProgram;
        
        DeferredLighting* deferredLighting;
        
        // Post-processing.
        PostProcessing* postProcessing;
        FXAAFilter* fxaaFilter;
        GammaCorrectionFilter* gammaCorrectionFilter;
        GlowFilter* glowFilter;
        GlowBlurFilter* glowBlurFilter;
        
        // Geometry.
        GLuint vertexBuffer;
        GLuint vertexArray;
};
