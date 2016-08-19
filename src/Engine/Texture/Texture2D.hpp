#pragma once

#include "Texture.hpp"
#include <glm/glm.hpp>
#include <string>
#include <json/json.h>

class Shader;
class ShaderProgram;
namespace Geometry {
    class Rectangle;
}

/// A two-dimensional texture.
/**
 * Used to load a 2D texture from an image file.
 */
class Texture2D : public Texture {
	public:
        /// Create new unloaded texture.
        Texture2D();
        
		/// Create new texture from the given image file.
		/**
	 	 * @param filename Filename (relative or absolute) of the image file.
	 	 * @param srgb Whether the image is in SRGB space and should be converted to linear space.
		 */
		Texture2D(const char* filename, bool srgb = false);
        
        /// Create new texture from given source string.
        /**
         * @param source Source string containing the image file.
         * @param sourceLength Length of the source string.
         * @param srgb Whether the image is in SRGB space and should be converted to linear space.
         */
        Texture2D(const char* source, int sourceLength, bool srgb = false);

		/// Destructor
		~Texture2D() override;

		/// Get OpenGL texture ID.
		/**
		 * Used when binding a texture before draw calls.
		 * @return The OpenGL texture identifier
		 */
		GLuint GetTextureID() const override;

		/// Get the width of the texture.
		/**
		 * @return The width of the texture in texels
		 */
		int GetWidth() const override;

		/// Get the height of the texture.
		/**
		 * @return The height of the texture in texels
		 */
		int GetHeight() const override;

		/// Set how coordinates outside 0..1 are handled.
		/**
		 * Default: GL_REPEAT
		 * @param wrapMode One of GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE or GL_CLAMP_TO_BORDER.
		 */
		void SetWrapping(GLint wrapMode);
        
        /// Render the texture to the screen.
        /**
         * @param position Position on the screen, in pixels.
         * @param size Size in pixels.
         * @param alpha Opacity (0.0 - 1.0).
         */
        void Render(const glm::vec2& position, const glm::vec2& size, float alpha = 1.f) const;
        
        /// Get whether the texture was created from file.
        /**
         * @return true if the texture was loaded from a file, false otherwise.
         */
        bool IsFromFile() const;
        
        /// Save the texture.
        /**
         * @return JSON value to be stored on disk.
         */
        Json::Value Save() const;
        
        /// Load texture from JSON node.
        /**
         * @param node JSON node to load from.
         */
        void Load(const Json::Value& node);
        
        /// Load texture from file.
        /**
         * @param filename Filename (relative or absolute) of the image file.
         * @param srgb Whether the image is in SRGB space and should be converted to linear space.
         */
        void Load(const char* filename, bool srgb = false);
        
        /// The name of the texture.
        std::string name;
        
        /// Whether to convert from SRGB space to linear space when loading.
        bool srgb = false;
        
	private:
		GLuint texID;
		int width, height;
        bool isFromFile;
        
        Geometry::Rectangle* rectangle;
        
        // Shaders
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
};
