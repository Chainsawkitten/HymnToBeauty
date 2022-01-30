#include "RenderPassAllocator.hpp"

#include "Texture.hpp"

namespace Video {

RenderPassAllocator::RenderPassAllocator() {

}

RenderPassAllocator::~RenderPassAllocator() {
    for (auto it : renderPasses) {
        delete it.second;
    }

    for (auto it : attachmentlessRenderPasses) {
        delete it.second;
    }
}

void RenderPassAllocator::BeginFrame() {
    /// @todo Free old render passes
}

RenderPass* RenderPassAllocator::CreateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) {
    RenderPassInfo renderPassInfo;
    renderPassInfo.colorAttachment = (colorAttachment == nullptr) ? 0u : colorAttachment->GetUniqueIdentifier();
    renderPassInfo.colorLoadOperation = colorLoadOperation;
    renderPassInfo.depthAttachment = (depthAttachment == nullptr) ? 0u : depthAttachment->GetUniqueIdentifier();
    renderPassInfo.depthLoadOperation = depthLoadOperation;

    auto it = renderPasses.find(renderPassInfo);
    if (it != renderPasses.end()) {
        // Use cached render pass.
        return it->second;
    } else {
        // Create render pass.
        RenderPass* renderPass = AllocateRenderPass(colorAttachment, colorLoadOperation, depthAttachment, depthLoadOperation);
        renderPasses[renderPassInfo] = renderPass;
        return renderPass;
    }
}

RenderPass* RenderPassAllocator::CreateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples) {
    AttachmentlessRenderPassInfo renderPassInfo;
    renderPassInfo.size = size;
    renderPassInfo.msaaSamples = msaaSamples;

    auto it = attachmentlessRenderPasses.find(renderPassInfo);
    if (it != attachmentlessRenderPasses.end()) {
        // Use cached render pass.
        return it->second;
    } else {
        // Create render pass.
        RenderPass* renderPass = AllocateAttachmentlessRenderPass(size, msaaSamples);
        attachmentlessRenderPasses[renderPassInfo] = renderPass;
        return renderPass;
    }
}

}
