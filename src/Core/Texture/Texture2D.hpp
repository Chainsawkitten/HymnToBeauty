#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include "Texture.hpp"

/** @ingroup Core
 * @{
 */

/// A two-dimensional texture.
/**
 * Used to load a 2D texture from an image file.
 */
class Texture2D : public Texture {
	public:
		/// Create new texture from the given image file.
		/**
		 * Supported image formats: TGA.
	 	 * @param filename Filename (relative or absolute) of the image file.
		 */
		Texture2D(const char* filename);

		/// Destructor
		~Texture2D();

		/// Get OpenGL texture ID.
		/**
		 * Used when binding a texture before draw calls.
		 * @return The OpenGL texture identifier
		 */
		GLuint TextureID() const;

		/// Get the width of the texture.
		/**
		 * @return The width of the texture in texels
		 */
		int Width() const;

		/// Get the height of the texture.
		/**
		 * @return The height of the texture in texels
		 */
		int Height() const;

		/// Set how coordinates outside 0..1 are handled.
		/**
		 * Default: GL_REPEAT
		 * @param wrapMode One of GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE or GL_CLAMP_TO_BORDER.
		 */
		void SetWrapping(GLint wrapMode);

	private:
		GLuint texID;
		int width, height;
};

/** @} */

#endif
