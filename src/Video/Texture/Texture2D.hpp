#pragma once

#include <glm/glm.hpp>

namespace Video {
class Texture;
class LowLevelRenderer;

/// A two-dimensional texture.
class Texture2D {
  public:
    /// Create new texture from file.
    /**
     * Supported formats: PNG, JPEG.
     * @param lowLevelRenderer The low-level renderer to use.
     * @param filename Name of the file to load.
     */
    Texture2D(LowLevelRenderer* lowLevelRenderer, const char* filename);

    /// Create new PNG texture from memory.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param source Source string containing the image file.
     * @param sourceLength Length of the source string.
     */
    Texture2D(LowLevelRenderer* lowLevelRenderer, const char* source, int sourceLength);

    /// Create new uniform texture with a specified value.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param size The size of the texture in pixels.
     * @param color The color value.
     */
    Texture2D(LowLevelRenderer* lowLevelRenderer, const glm::uvec2& size, const glm::vec4& color);

    /// Destructor.
    ~Texture2D();

    /// Get the low-level texture.
    /**
     * @return The low-level texture.
     */
    Texture* GetTexture();

    /// Get whether the texture has been loaded yet.
    /**
     * @return Whether the texture has been loaded yet.
     */
    bool IsLoaded() const;

  protected:
    /// Create new texture.
    Texture2D();

  private:
    Texture2D(const Texture2D& other) = delete;

    Texture* texture = nullptr;
    bool loaded = false;
};
} // namespace Video
