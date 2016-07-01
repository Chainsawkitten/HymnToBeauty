#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader;
class ShaderProgram;
namespace Geometry {
    class Rectangle;
}

/// Render target containing a color and depth buffer.
class RenderTarget {
    public:
        /// Create new render target.
		/**
		 * @param size Size of the new render target.
		 */
		RenderTarget(const glm::vec2& size);
        
        /// Destructor.
        ~RenderTarget();
        
        /// Set as render target.
		void SetTarget();
        
        /// Get size of the render target.
        /**
         * @return The size of the render target in pixels.
         */
        glm::vec2 GetSize() const;
        
        /// Get color texture.
        /**
         * @return The color buffer's texture.
         */
        GLuint GetColorTexture() const;
        
        /// Get extra texture.
        /**
         * Used for extra information, e.g. for bloom.
         * @return The extra buffer's texture.
         */
        GLuint GetExtraTexture() const;
        
        /// Get depth texture.
        /**
         * @return The depth buffer's texture.
         */
        GLuint GetDepthTexture() const;
        
        /// Render resulting image to screen.
        void Render();
        
    private:
        unsigned int width;
		unsigned int height;
        
        GLuint frameBuffer;
		GLuint colorBuffer;
        GLuint extraBuffer;
		GLuint depthBuffer;
        
        Geometry::Rectangle* rectangle;
        
        // Shaders
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
};
