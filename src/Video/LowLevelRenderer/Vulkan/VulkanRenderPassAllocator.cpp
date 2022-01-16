#include "VulkanRenderPassAllocator.hpp"

#include "VulkanRenderPass.hpp"

namespace Video {

VulkanRenderPassAllocator::VulkanRenderPassAllocator(VkDevice device) {
    this->device = device;
}

VulkanRenderPassAllocator::~VulkanRenderPassAllocator() {

}

RenderPass* VulkanRenderPassAllocator::AllocateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) {
    return new VulkanRenderPass(device, colorAttachment, colorLoadOperation, depthAttachment, depthLoadOperation);
}

RenderPass* VulkanRenderPassAllocator::AllocateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples) {
    return new VulkanRenderPass(device, size, msaaSamples);
}

}
