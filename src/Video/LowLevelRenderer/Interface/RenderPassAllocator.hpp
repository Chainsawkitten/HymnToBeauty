#pragma once

#include "RenderPass.hpp"
#include <map>

namespace Video {

class Texture;

/// Responsible for allocating render passes and framebuffers.
class RenderPassAllocator {
  public:
    /// Create a new render pass allocator.
    RenderPassAllocator();

    /// Destructor.
    virtual ~RenderPassAllocator();

    /// Call at the beginning of each frame.
    void BeginFrame();

    /// Create a render pass.
    /**
     * @param colorAttachment The color attachment to draw to.
     * @param colorLoadOperation What to do with the previous contents of the color attachment.
     * @param depthAttachment The depth attachment to draw to.
     * @param depthLoadOperation What to do with the previous contents of the depth attachment.
     */
    RenderPass* CreateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation);

    /// Create an attachmentless render pass.
    /**
     * @param size The framebuffer size.
     * @param msaaSamples Number of MSAA samples.
     */
    RenderPass* CreateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples);

  private:
    struct RenderPassInfo {
        uint64_t colorAttachment;
        RenderPass::LoadOperation colorLoadOperation;
        uint64_t depthAttachment;
        RenderPass::LoadOperation depthLoadOperation;
    };

    struct RenderPassInfoCompare {
        bool operator()(const RenderPassInfo& a, const RenderPassInfo& b) const {
            if (a.colorAttachment != b.colorAttachment) {
                return a.colorAttachment < b.colorAttachment;
            }

            if (a.colorLoadOperation != b.colorLoadOperation) {
                return a.colorLoadOperation < b.colorLoadOperation;
            }

            if (a.depthAttachment != b.depthAttachment) {
                return a.depthAttachment < b.depthAttachment;
            }

            return a.depthLoadOperation < b.depthLoadOperation;
        }
    };

    std::map<RenderPassInfo, RenderPass*, RenderPassInfoCompare> renderPasses;

    struct AttachmentlessRenderPassInfo {
        glm::uvec2 size;
        uint32_t msaaSamples;
    };

    struct AttachmentlessRenderPassInfoCompare {
        bool operator()(const AttachmentlessRenderPassInfo& a, const AttachmentlessRenderPassInfo& b) const {
            if (a.size.x != b.size.x) {
                return a.size.x < b.size.x;
            }

            if (a.size.y != b.size.y) {
                return a.size.y < b.size.y;
            }

            return a.msaaSamples < b.msaaSamples;
        }
    };

    std::map<AttachmentlessRenderPassInfo, RenderPass*, AttachmentlessRenderPassInfoCompare> attachmentlessRenderPasses;

    virtual RenderPass* AllocateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) = 0;
    virtual RenderPass* AllocateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples) = 0;
};

}
