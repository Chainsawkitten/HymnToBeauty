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
        R8, ///< 8-bit single channel color format.
        R8G8B8A8, ///< 8-bit RGBA color format.
        R11G11B10, ///< HDR RGB color format.
        R16G16B16A16, ///< HDR RGBA color format.
        D32 ///< 32-bit depth format.
    };

    /// Create a new texture.
    /**
     * @param type The type of texture.
     * @param size The size of the texture.
     * @param format The format of the texture.
     */
    Texture(Type type, const glm::uvec2& size, Format format);

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

    /// Get the format of the texture.
    /**
     * @return The format of the texture.
     */
    Format GetFormat() const {
        return format;
    }

    /// Get a unique identifier.
    /**
     * @return A unique identifier.
     */
    uint64_t GetUniqueIdentifier() const;

  private:
    Texture(const Texture& other) = delete;

    Type type;
    Format format;
    glm::uvec2 size;
    uint64_t uniqueIdentifier;
};

}
