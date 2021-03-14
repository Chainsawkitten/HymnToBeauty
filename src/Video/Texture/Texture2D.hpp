#pragma once

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

    /// Destructor.
    ~Texture2D();

    /// Get the low-level texture.
    /**
     * @return The low-level texture.
     */
    const Texture* GetTexture() const;

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
