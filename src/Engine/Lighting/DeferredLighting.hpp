#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

namespace Geometry {
    class Rectangle;
}
class Scene;
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
		/**
		 * @param size Size of the context.
		 */
		DeferredLighting(const glm::vec2& size);
        
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
        
        /// Render the lighting in the scene.
		/**
         * @param scene Scene to get the lights from.
		 * @param camera Camera to use.
		 * @param screenSize Size of the screen in pixels.
		 */
		void Render(Scene& scene, Entity* camera, const glm::vec2& screenSize);
        
	private:
        static void AttachTexture(GLuint texture, unsigned int width, unsigned int height, GLenum attachment, GLint internalFormat);
        void BindForReading();
        
		GLuint textures[NUM_TEXTURES];
        
		GLuint frameBufferObject;
		GLuint depthHandle;
        
		glm::vec2 size;
        
        Geometry::Rectangle* rectangle;
        
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
};
