#include "VulkanRenderPass.hpp"

#include <Utility/Log.hpp>
#include "VulkanTexture.hpp"

namespace Video {

VulkanRenderPass::VulkanRenderPass(VkDevice device, Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) {
    assert(colorAttachment != nullptr || depthAttachment != nullptr);

    this->device = device;
    VulkanTexture* vulkanColorAttachment = nullptr;
    hasColorAttachment = colorAttachment != nullptr;
    if (hasColorAttachment) {
        assert(colorAttachment->GetType() == Texture::Type::RENDER_COLOR);
        vulkanColorAttachment = static_cast<VulkanTexture*>(colorAttachment);
    }
    this->colorAttachment = vulkanColorAttachment;

    VulkanTexture* vulkanDepthAttachment = nullptr;
    hasDepthAttachment = depthAttachment != nullptr;
    if (hasDepthAttachment) {
        assert(depthAttachment->GetType() == Texture::Type::RENDER_DEPTH);
        vulkanDepthAttachment = static_cast<VulkanTexture*>(depthAttachment);
    }
    this->depthAttachment = vulkanDepthAttachment;

    // Attachments.
    VkAttachmentDescription attachmentDescriptions[2] = {};
    VkImageView imageViews[2];
    uint32_t attachmentNumber = 0;

    // Color attachment.
    uint32_t colorAttachmentIndex = attachmentNumber;
    if (hasColorAttachment) {
        attachmentDescriptions[attachmentNumber].format = vulkanColorAttachment->GetFormat();
        attachmentDescriptions[attachmentNumber].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescriptions[attachmentNumber].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescriptions[attachmentNumber].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescriptions[attachmentNumber].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescriptions[attachmentNumber].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachmentDescriptions[attachmentNumber].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        switch (colorLoadOperation) {
        case RenderPass::LoadOperation::LOAD:
            attachmentDescriptions[attachmentNumber].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            break;
        case RenderPass::LoadOperation::CLEAR:
            attachmentDescriptions[attachmentNumber].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            break;
        case RenderPass::LoadOperation::DONT_CARE:
            attachmentDescriptions[attachmentNumber].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            break;
        }

        imageViews[attachmentNumber] = vulkanColorAttachment->GetImageView();

        size = vulkanColorAttachment->GetSize();

        attachmentNumber++;
    }

    // Depth attachment.
    uint32_t depthAttachmentIndex = attachmentNumber;
    if (hasDepthAttachment) {
        attachmentDescriptions[attachmentNumber].format = vulkanDepthAttachment->GetFormat();
        attachmentDescriptions[attachmentNumber].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescriptions[attachmentNumber].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescriptions[attachmentNumber].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescriptions[attachmentNumber].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescriptions[attachmentNumber].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachmentDescriptions[attachmentNumber].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        switch (depthLoadOperation) {
        case RenderPass::LoadOperation::LOAD:
            attachmentDescriptions[attachmentNumber].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
            break;
        case RenderPass::LoadOperation::CLEAR:
            attachmentDescriptions[attachmentNumber].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            break;
        case RenderPass::LoadOperation::DONT_CARE:
            attachmentDescriptions[attachmentNumber].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            break;
        }

        imageViews[attachmentNumber] = vulkanDepthAttachment->GetImageView();

        size = vulkanDepthAttachment->GetSize();

        attachmentNumber++;
    }

    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = colorAttachmentIndex;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = depthAttachmentIndex;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Subpass.
    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    if (hasColorAttachment) {
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentReference;
    }
    if (hasDepthAttachment) {
        subpass.pDepthStencilAttachment = &depthAttachmentReference;
    }

    // Subpass dependency.
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;

    if (hasColorAttachment) {
        dependency.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if (hasDepthAttachment) {
        dependency.srcStageMask |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        dependency.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    // Create render pass.
    VkRenderPassCreateInfo renderPassCreateInfo = {};
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = attachmentNumber;
    renderPassCreateInfo.pAttachments = attachmentDescriptions;
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;
    renderPassCreateInfo.dependencyCount = 1;
    renderPassCreateInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create render pass.\n";
    }

    // Create framebuffer.
    VkFramebufferCreateInfo framebufferCreateInfo = {};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.renderPass = renderPass;
    framebufferCreateInfo.attachmentCount = attachmentNumber;
    framebufferCreateInfo.pAttachments = imageViews;
    framebufferCreateInfo.width = size.x;
    framebufferCreateInfo.height = size.y;
    framebufferCreateInfo.layers = 1;

    if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create framebuffer.\n";
    }

    // Compatiblity information.
    compatibility.hasColorAttachment = hasColorAttachment;
    if (hasColorAttachment)
        compatibility.colorAttachmentFormat = vulkanColorAttachment->GetFormat();

    compatibility.hasDepthAttachment = hasDepthAttachment;
    if (hasDepthAttachment)
        compatibility.depthAttachmentFormat = vulkanDepthAttachment->GetFormat();
}

VulkanRenderPass::~VulkanRenderPass() {
    vkDestroyFramebuffer(device, framebuffer, nullptr);
    vkDestroyRenderPass(device, renderPass, nullptr);
}

const glm::uvec2& VulkanRenderPass::GetSize() const {
    return size;
}

VkRenderPass VulkanRenderPass::GetRenderPass() const {
    return renderPass;
}

VkFramebuffer VulkanRenderPass::GetFramebuffer() const {
    return framebuffer;
}

bool VulkanRenderPass::HasColorAttachment() const {
    return hasColorAttachment;
}

VulkanTexture* VulkanRenderPass::GetColorAttachment() const {
    assert(hasColorAttachment);

    return colorAttachment;
}

bool VulkanRenderPass::HasDepthAttachment() const {
    return hasDepthAttachment;
}

VulkanTexture* VulkanRenderPass::GetDepthAttachment() const {
    assert(hasDepthAttachment);

    return depthAttachment;
}

const VulkanRenderPass::Compatibility& VulkanRenderPass::GetCompatiblity() const {
    return compatibility;
}

}
