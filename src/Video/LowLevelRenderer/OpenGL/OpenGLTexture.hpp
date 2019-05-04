#pragma once

#include "../Interface/Texture.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace Video {

/// OpenGL implementation of Texture.
class OpenGLTexture : public Texture {
  public:
    /// Create new OpenGL texture.
    /**
     * @param size The size of the texture, in pixels.
     * @param type The type of texture to create.
     * @param format The format of the texture.
     * @param components The number of components in the texture, 0 if no texture data is supplied.
     * @param data The texture data to upload, or nullptr.
     */
    OpenGLTexture(const glm::uvec2 size, Texture::Type type, Texture::Format format, int components = 0, unsigned char* data = nullptr);

    /// Destructor.
    ~OpenGLTexture() final;

    /// Get texture ID.
    /**
     * @return The ID used in OpenGL calls.
     */
    unsigned int GetID() const;

  private:
    OpenGLTexture(const OpenGLTexture& other) = delete;

    // Get image GL format based on color components.
    static GLenum Format(int components);

    GLuint texture;
};

}
