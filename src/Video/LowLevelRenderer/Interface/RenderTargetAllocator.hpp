#pragma once

#include "Texture.hpp"
#include <glm/glm.hpp>
#include <map>
#include <deque>

namespace Video {

class RenderPassAllocator;

/// Responsible for allocating render targets.
class RenderTargetAllocator {
  public:
    /// Create a new render target allocator.
    /**
     * @param frames How many frames to keep unused render targets alive.
     * @param renderPassAllocator Render pass allocator.
     */
    RenderTargetAllocator(uint8_t frames, RenderPassAllocator* renderPassAllocator);

    /// Destructor.
    virtual ~RenderTargetAllocator();

    /// Call at the beginning of each frame.
    void BeginFrame();

    /// Create a render target.
    /**
     * @param size The size of the texture, in pixels.
     * @param format The format of the texture.
     */
    Texture* CreateRenderTarget(const glm::uvec2& size, Texture::Format format);

    /// Free a render target.
    /**
     * @param renderTarget The render target to free.
     */
    void FreeRenderTarget(Texture* renderTarget);

  private:
    struct RenderTargetInfo {
        glm::uvec2 size;
        Texture::Format format;
    };

    struct RenderTargetInfoCompare {
        bool operator()(const RenderTargetInfo& a, const RenderTargetInfo& b) const {
            if (a.size.x != b.size.x) {
                return a.size.x < b.size.x;
            }

            if (a.size.y != b.size.y) {
                return a.size.y < b.size.y;
            }

            return a.format < b.format;
        }
    };

    struct RenderTarget {
        Texture* texture;
        uint8_t age;
    };

    std::map<RenderTargetInfo, std::deque<RenderTarget>, RenderTargetInfoCompare> freeRenderTargets;
    uint8_t frames; 
    RenderPassAllocator* renderPassAllocator;

    virtual Texture* AllocateRenderTarget(const glm::uvec2& size, Texture::Format format) = 0;
};

}
