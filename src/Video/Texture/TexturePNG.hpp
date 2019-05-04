#pragma once

#include "Texture2D.hpp"

namespace Video {
class LowLevelRenderer;

/// Texture loaded from a PNG file.
class TexturePNG : public Texture2D {
  public:
    /// Create new PNG texture from file.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param filename Name of the PNG file to load.
     */
    TexturePNG(LowLevelRenderer* lowLevelRenderer, const char* filename);

    /// Create new PNG texture from memory.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param source Source string containing the image file.
     * @param sourceLength Length of the source string.
     */
    TexturePNG(LowLevelRenderer* lowLevelRenderer, const char* source, int sourceLength);

    /// Destructor.
    ~TexturePNG() override;

    const Texture* GetTexture() const override;
    bool IsLoaded() const override;

  private:
    Texture* texture = nullptr;
    bool loaded = false;
};
} // namespace Video
