#pragma once

namespace Video {
class Texture;

/// A two-dimensional texture.
class Texture2D {
  public:
    /// Destructor
    virtual ~Texture2D();

    /// Get the low-level texture.
    /**
     * @return The low-level texture.
     */
    virtual const Texture* GetTexture() const = 0;

    /// Get whether the texture has been loaded yet.
    /**
     * @return Whether the texture has been loaded yet.
     */
    virtual bool IsLoaded() const = 0;

  protected:
    /// Create new texture.
    Texture2D();

  private:
    Texture2D(const Texture2D& other) = delete;
};
} // namespace Video
