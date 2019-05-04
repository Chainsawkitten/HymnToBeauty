#include "OpenGLTexture.hpp"

#include <glad/glad.h>
#include <assert.h>

namespace Video {

OpenGLTexture::OpenGLTexture(const glm::uvec2 size, Texture::Type type, Texture::Format format, int components, unsigned char* data) : Texture(type, size) {
    assert(size.x > 0 && size.y > 0);
    assert((data != nullptr && components >= 1 && components <= 4) || (data == nullptr && components == 0));
    assert((type == Texture::Type::COLOR && data != nullptr) || (type != Texture::Type::COLOR && data == nullptr));

    // Get OpenGL internal format.
    GLint internalFormat;
    GLenum dataFormat;
    switch (format) {
    case Texture::Format::R8G8B8A8:
        assert(type != Texture::Type::RENDER_DEPTH);
        internalFormat = GL_RGBA8;
        dataFormat = Format(components);
        break;
    case Texture::Format::R11G11B10:
        assert(type != Texture::Type::RENDER_DEPTH);
        internalFormat = GL_R11F_G11F_B10F;
        dataFormat = Format(components);
        break;
    case Texture::Format::D32:
        assert(type == Texture::Type::RENDER_DEPTH);
        internalFormat = GL_DEPTH_COMPONENT32;
        dataFormat = GL_DEPTH_COMPONENT;
        break;
    default:
        assert(false);
    }

    // Create texture.
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Upload the image data.
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, size.x, size.y, 0, dataFormat, GL_UNSIGNED_BYTE, (data != nullptr) ? data : NULL);

    /// @todo Configurable filtering.
    switch (type) {
    case Texture::Type::COLOR:
        // Set texture parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Generate mipmaps.
        glGenerateMipmap(GL_TEXTURE_2D);
        break;
    case Texture::Type::RENDER_COLOR:
    case Texture::Type::RENDER_DEPTH:
        // Set texture parameters.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        break;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

OpenGLTexture::~OpenGLTexture() {
    glDeleteTextures(1, &texture);
}

unsigned int OpenGLTexture::GetID() const {
    return texture;
}

GLenum OpenGLTexture::Format(int components) {
    switch (components) {
    case 1:
        return GL_RED;
    case 2:
        return GL_RG;
    case 3:
        return GL_RGB;
    default:
        return GL_RGBA;
    }
}

}
