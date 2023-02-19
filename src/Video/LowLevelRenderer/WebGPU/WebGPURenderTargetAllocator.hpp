#pragma once

#include "../Interface/RenderTargetAllocator.hpp"

#include <dawn/webgpu.h>
#include <map>
#include <deque>

namespace Video {

class WebGPURenderer;
class WebGPUTexture;

/// WebGPU implementation of RenderTargetAllocator.
class WebGPURenderTargetAllocator : public RenderTargetAllocator {
  public:
    /// Create a new render target allocator.
    /**
     * @param renderer The WebGPU renderer.
     */
    explicit WebGPURenderTargetAllocator(WebGPURenderer& renderer);

    /// Destructor.
    ~WebGPURenderTargetAllocator() final;

    void BeginFrame() final;

    /// Create an attachmentless render target.
    /**
     * @param size The size of the texture, in pixels.
     * @param sampleCount MSAA samples.
     */
    WebGPUTexture* CreateAttachmentlessRenderTarget(const glm::uvec2& size, uint32_t sampleCount = 1);

    /// Free an attachmentless render target.
    /**
     * @param renderTarget The render target to free.
     */
    void FreeAttachmentlessRenderTarget(WebGPUTexture* renderTarget);

  private:
    struct AttachmentlessRenderTargetInfo {
        glm::uvec2 size;
        uint32_t sampleCount;
    };

    struct AttachmentlessRenderTargetInfoCompare {
        bool operator()(const AttachmentlessRenderTargetInfo& a, const AttachmentlessRenderTargetInfo& b) const {
            if (a.size.x != b.size.x) {
                return a.size.x < b.size.x;
            }

            if (a.size.y != b.size.y) {
                return a.size.y < b.size.y;
            }

            return a.sampleCount < b.sampleCount;
        }
    };

    struct AttachmentlessRenderTarget {
        WebGPUTexture* texture;
        uint8_t age;
    };

    Texture* AllocateRenderTarget(const glm::uvec2& size, Texture::Format format) final;

    // Since we don't have a render pass allocator, we need to override this method.
    void FreeRenderPasses(Texture* renderTarget) final;

    WebGPURenderer& renderer;

    std::map<AttachmentlessRenderTargetInfo, std::deque<AttachmentlessRenderTarget>, AttachmentlessRenderTargetInfoCompare> freeAttachmentlessRenderTargets;
};

}
