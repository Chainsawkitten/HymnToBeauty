#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class World;
class Entity;
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

/// Handles rendering the world.
class RenderManager {
    friend class Hub;
    
    public:
        /// Render world containing entities.
        /**
         * @param world Contains a bunch of entities.
         * @param camera Camera through which to render (or first camera in the world if nullptr).
         */
        void Render(World& world, Entity* camera = nullptr);
        
        /// Render editor entities.
        /**
         * @param world World to render.
         * @param camera Camera through which to render (or first camera in the world if nullptr).
         * @param soundSources Whether to show sound sources.
         * @param particleEmitters Whether to show particle emitters.
         * @param lightSources Whether to show light sources.
         * @param cameras Whether to show cameras.
         */
        void RenderEditorEntities(World& world, Entity* camera = nullptr, bool soundSources = true, bool particleEmitters = true, bool lightSources = true, bool cameras = true);
        
        /// Updates the buffers to fit the current screen size.
        void UpdateBufferSize();
        
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
        Texture2D* cameraTexture;
        
        // Geometry.
        GLuint vertexBuffer;
        GLuint vertexArray;
};
