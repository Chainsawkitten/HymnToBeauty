#pragma once

#include <GL/glew.h>

namespace Geometry {
    class Rectangle;
}
class World;
class Entity;
class Shader;
class ShaderProgram;

/// Holds the frame buffers used for deferred rendering.
class DeferredLighting {
    public:
        /// The different buffer types.
        enum TEXTURE_TYPE {
            DIFFUSE, ///< Diffuse texture
            NORMAL, ///< Normals
            SPECULAR, ///< Specular
            GLOW, ///< Glow
            NUM_TEXTURES ///< Total number of textures (excluding depth buffer)
        };
        
        /// Create new render target.
        DeferredLighting();
        
        /// Destructor
        /**
         * Free allocated resources.
         */
        ~DeferredLighting();
        
        /// Set as render target.
        void SetTarget();
        
        /// Reset render target.
        /**
         * Needs to be called before rendering to the screen.
         */
        void ResetTarget();
        
        /// Render the lighting in the world.
        /**
         * @param world World to get the lights from.
         * @param camera Camera to use.
         */
        void Render(World& world, const Entity* camera);
        
    private:
        static void AttachTexture(GLuint texture, unsigned int width, unsigned int height, GLenum attachment, GLint internalFormat);
        void BindForReading();
        
        GLuint textures[NUM_TEXTURES];
        
        GLuint frameBufferObject;
        GLuint depthHandle;
        
        Geometry::Rectangle* rectangle;
        
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
        
        // Store light uniform locations so we don't have to get them every frame.
        static const unsigned int lightCount = 32U;
        struct {
            GLint position;
            GLint intensities;
            GLint attenuation;
            GLint ambientCoefficient;
            GLint coneAngle;
            GLint direction;
        } lightUniforms[lightCount];
};
