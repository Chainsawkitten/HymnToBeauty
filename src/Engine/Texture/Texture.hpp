#pragma once

#include <gl/glew.h>

/// Interface for textures.
/**
 * Used to load textures from image files.
 */
class Texture {
	public:
		/// Constructor
		Texture() { }

		/// Destructor
		virtual ~Texture() { }

		/// Get OpenGL texture ID.
		/**
		 * Used when binding a texture before draw calls.
		 * @return The OpenGL texture identifier
		 */
		virtual GLuint TextureID() const = 0;

		/// Get the width of the texture.
		/**
		 * @return The width of the texture in texels
		 */
		virtual int Width() const = 0;

		/// Get the height of the texture.
		/**
		 * @return The height of the texture in texels
		 */
		virtual int Height() const = 0;

		/// Get image GL format based on color components.
		/**
		 * @param components The amount of components in the image. (1, 2, 3, or 4.)
		 * @return One of GL_RED, GL_RG, GL_RGB or GL_RGBA
		 */
		static GLenum Format(int components);
};
