#pragma once

#include "Texture2D.hpp"

namespace Video {
/// Texture loaded from a PNG file.
class TexturePNG : public Texture2D {
  public:
    /// Create new PNG texture from file.
    /**
     * @param filename Name of the PNG file to load.
     */
    TexturePNG(const char* filename);

    /// Create new PNG texture from memory.
    /**
     * @param source Source string containing the image file.
     * @param sourceLength Length of the source string.
     */
    TexturePNG(const char* source, int sourceLength);

    /// Destructor.
    ~TexturePNG() override;

    GLuint GetTextureID() const override;
    bool IsLoaded() const override;

  private:
    // Get image GL format based on color components.
    static GLenum Format(int components);

    GLuint texID = 0;
    bool loaded = false;
};
} // namespace Video
