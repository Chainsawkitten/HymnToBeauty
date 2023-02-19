#pragma once

#include "../Interface/Texture.hpp"

#include <glm/glm.hpp>
#include <dawn/webgpu.h>

namespace Video {

class WebGPURenderer;

/// WebGPU implementation of Texture.
class WebGPUTexture : public Texture {
  public:
    /// Create new WebGPU texture.
    /**
     * @param renderer The WebGPU renderer.
     * @param size The size of the texture, in pixels.
     * @param type The type of texture to create.
     * @param format The format of the texture.
     * @param components The number of components in the texture, 0 if no texture data is supplied.
     * @param data The texture data to upload, or nullptr.
     * @param sampleCount MSAA samples.
     */
    WebGPUTexture(WebGPURenderer& renderer, const glm::uvec2 size, Texture::Type type, Texture::Format format, int components = 0, unsigned char* data = nullptr, uint32_t sampleCount = 1);

    /// Destructor.
    ~WebGPUTexture() final;

    /// Get the interan WebGPU texture.
    /**
     * @return The WebGPU texture.
     */
    WGPUTexture GetTexture() const;

    /// Get the texture view.
    /**
     * @return The texture view.
     */
    WGPUTextureView GetTextureView() const;

    /// Get the format of the texture.
    /**
     * @return The format of the texture.
     */
    WGPUTextureFormat GetTextureFormat() const;

    /// Get the MSAA sample count.
    /**
     * @return The sample count.
     */
    uint32_t GetSampleCount() const;

  private:
    WebGPUTexture(const WebGPUTexture& other) = delete;

    void GenerateMipMaps(WebGPURenderer& renderer, const glm::uvec2& size, uint32_t mipLevels);

    WGPUTexture texture;
    WGPUTextureView textureView;
    WGPUTextureFormat textureFormat;
    uint32_t sampleCount;
};

}
