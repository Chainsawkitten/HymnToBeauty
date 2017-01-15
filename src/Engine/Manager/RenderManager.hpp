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
class Texture2D;
namespace Component {
    class SuperComponent;
}

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
         * @param soundSources Whether to show sound sources.
         * @param particleEmitters Whether to show particle emitters.
         * @param lightSources Whether to show light sources.
         */
        void RenderEditorEntities(Scene& scene, bool soundSources = true, bool particleEmitters = true, bool lightSources = true);
        
    private:
        RenderManager();
        ~RenderManager();
        RenderManager(RenderManager const&) = delete;
        void operator=(RenderManager const&) = delete;
        
        void RenderEditorEntity(Component::SuperComponent* component);
        
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
        
        // Editor entity textures.
        Texture2D* particleEmitterTexture;
        Texture2D* lightTexture;
        Texture2D* soundSourceTexture;
        
        // Geometry.
        GLuint vertexBuffer;
        GLuint vertexArray;
};
