#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Video {
class LowLevelRenderer;
class Texture2D;
}

/// A texture used in a hymn.
class TextureAsset {
  public:
    /// Create new texture asset.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     */
    explicit TextureAsset(Video::LowLevelRenderer* lowLevelRenderer);

    /// Create new texture asset from a PNG image.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param source Source string containing the image file.
     * @param sourceLength Length of the source string.
     */
    TextureAsset(Video::LowLevelRenderer* lowLevelRenderer, const char* source, int sourceLength);

    /// Create new uniform texture asset with a specified color value.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param size The size of the texture in pixels.
     * @param color The color value.
     */
    TextureAsset(Video::LowLevelRenderer* lowLevelRenderer, const glm::uvec2& size, const glm::vec4& color);

    /// Destructor.
    ~TextureAsset();

    /// Save the texture asset.
    void Save() const;

    /// Load texture asset from disk.
    /**
     * @param name The name of the texture asset.
     */
    void Load(const std::string& name);

    /// Get the texture.
    /**
     * @return The texture.
     */
    Video::Texture2D* GetTexture() const;

    /// The name of the texture.
    std::string name;

    /// The folder containing the texture files.
    std::string path;

  private:
    TextureAsset(const TextureAsset& other) = delete;

    Video::LowLevelRenderer* lowLevelRenderer;
    Video::Texture2D* texture;
};
