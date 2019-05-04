#pragma once

#include <glm/glm.hpp>

namespace Video {

/// A texture.
class Texture {
  public:
    /// The type of texture.
    enum class Type {
        COLOR, ///< A sampled color texture.
        RENDER_COLOR, ///< A color texture that can be rendered to.
        RENDER_DEPTH ///< A depth texture that can be rendered to.
    };

    /// The format of the texture.
    enum class Format {
        R8G8B8A8, ///< 8-bit RGBA color format.
        R11G11B10, ///< HDR RGB color format.
        D32 ///< 32-bit depth format.
    };

    /// Create a new texture.
    /**
     * @param type The type of texture.
     * @param size The size of the texture.
     */
    Texture(Type type, const glm::uvec2& size) {
        this->type = type;
        this->size = size;
    }

    /// Destructor.
    virtual ~Texture() {}

    /// Get the type of texture.
    /**
     * @return The type of the texture.
     */
    Type GetType() const {
        return type;
    }

    /// Get the size of the texture.
    /**
     * @return The size of the texture in pixels.
     */
    const glm::uvec2& GetSize() const {
        return size;
    }

  private:
    Texture(const Texture& other) = delete;

    Type type;
    glm::uvec2 size;
};

}
