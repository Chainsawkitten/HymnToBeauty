#include "VulkanRenderTargetAllocator.hpp"

#include "VulkanTexture.hpp"
#include "VulkanRenderer.hpp"

#include <cassert>

namespace Video {

VulkanRenderTargetAllocator::VulkanRenderTargetAllocator(VulkanRenderer& vulkanRenderer, VkDevice device, VkPhysicalDevice physicalDevice, RenderPassAllocator* renderPassAllocator) : RenderTargetAllocator(vulkanRenderer.GetSwapChainImageCount(), renderPassAllocator), vulkanRenderer(vulkanRenderer) {
    assert(renderPassAllocator != nullptr);

    this->device = device;
    this->physicalDevice = physicalDevice;
}

VulkanRenderTargetAllocator::~VulkanRenderTargetAllocator() {

}

Texture* VulkanRenderTargetAllocator::AllocateRenderTarget(const glm::uvec2& size, Texture::Format format) {
    Texture::Type type;
    switch (format) {
    case Texture::Format::R8:
    case Texture::Format::R11G11B10:
    case Texture::Format::R8G8B8A8:
    case Texture::Format::R16G16B16A16:
        type = Texture::Type::RENDER_COLOR;
        break;
    case Texture::Format::D32:
        type = Texture::Type::RENDER_DEPTH;
        break;
    default:
        assert(false);
    }

    return new VulkanTexture(vulkanRenderer, device, physicalDevice, size, type, format);
}

}
