#include "VulkanCommandBuffer.hpp"

#include <Utility/Log.hpp>
#include "VulkanRenderer.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanGeometryBinding.hpp"
#include "VulkanShaderProgram.hpp"
#include "VulkanTexture.hpp"
#include "VulkanUtility.hpp"

namespace Video {

VulkanCommandBuffer::VulkanCommandBuffer(VulkanRenderer* vulkanRenderer, VkDevice device, VkCommandPool commandPool) {
    this->vulkanRenderer = vulkanRenderer;
    this->device = device;
    this->commandPool = commandPool;

    // Setup info.
    renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderArea.offset.x = 0;
    renderPassBeginInfo.renderArea.offset.y = 0;
    renderPassBeginInfo.pClearValues = clearValues;

    swapChainImages = vulkanRenderer->GetSwapChainImageCount();
    commandBuffer = new VkCommandBuffer[swapChainImages];
    secondaryCommandBuffers = new std::vector<VkCommandBuffer>[swapChainImages];

    // Allocate command buffer.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = swapChainImages;

    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffer) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create command buffer.\n";
    }

    Begin();
}

VulkanCommandBuffer::~VulkanCommandBuffer() {
    vkFreeCommandBuffers(device, commandPool, swapChainImages, commandBuffer);
    for (uint32_t i = 0; i < swapChainImages; ++i) {
        if (!secondaryCommandBuffers[i].empty())
            vkFreeCommandBuffers(device, commandPool, secondaryCommandBuffers[i].size(), secondaryCommandBuffers[i].data());
    }

    delete[] commandBuffer;
}

void VulkanCommandBuffer::BeginRenderPass(RenderPass* renderPass, const std::string& name) {
    assert(!inRenderPass);
    assert(renderPass != nullptr);

    currentRenderPass = static_cast<VulkanRenderPass*>(renderPass);

    // Image memory barriers.
    if (currentRenderPass->HasColorAttachment()) {
        TransitionTexture(currentRenderPass->GetColorAttachment(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    }

    if (currentRenderPass->HasDepthAttachment()) {
        TransitionTexture(currentRenderPass->GetDepthAttachment(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
    }

    // Begin render pass info.
    uint32_t clearNumber = 0;

    if (currentRenderPass->HasColorAttachment()) {
        clearValues[clearNumber].color.float32[0] = 0.0f;
        clearValues[clearNumber].color.float32[1] = 0.0f;
        clearValues[clearNumber].color.float32[2] = 0.0f;
        clearValues[clearNumber].color.float32[3] = 1.0f;

        clearNumber++;
    }

    if (currentRenderPass->HasDepthAttachment()) {
        clearValues[clearNumber].depthStencil.depth = 1.0f;
        clearValues[clearNumber].depthStencil.stencil = 0;

        clearNumber++;
    }

    renderPassBeginInfo.renderPass = currentRenderPass->GetRenderPass();
    renderPassBeginInfo.framebuffer = currentRenderPass->GetFramebuffer();
    renderPassBeginInfo.renderArea.extent.width = currentRenderPass->GetSize().x;
    renderPassBeginInfo.renderArea.extent.height = currentRenderPass->GetSize().y;
    renderPassBeginInfo.clearValueCount = clearNumber;

    // Allocate secondary command buffer.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &renderPassCommandBuffer) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create command buffer.\n";
    }

    secondaryCommandBuffers[currentFrame].push_back(renderPassCommandBuffer);

    // Begin secondary command buffer.
    VkCommandBufferInheritanceInfo inheritanceInfo = {};
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inheritanceInfo.renderPass = currentRenderPass->GetRenderPass();
    inheritanceInfo.subpass = 0;
    inheritanceInfo.framebuffer = currentRenderPass->GetFramebuffer();

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
    beginInfo.pInheritanceInfo = &inheritanceInfo;

    if (vkBeginCommandBuffer(renderPassCommandBuffer, &beginInfo) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to begin command buffer.\n";
    }

    // Queries.
    if (vulkanRenderer->IsProfiling()) {
        Timing timing;
        timing.name = name;
        timing.startQuery = vulkanRenderer->GetFreeQuery();
        timing.endQuery = vulkanRenderer->GetFreeQuery();

        timings.push_back(timing);
    }

    inRenderPass = true;
}

void VulkanCommandBuffer::EndRenderPass() {
    assert(inRenderPass);

    vkEndCommandBuffer(renderPassCommandBuffer);

    if (vulkanRenderer->IsProfiling()) {
        vkCmdWriteTimestamp(commandBuffer[currentFrame], VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, vulkanRenderer->GetQueryPool(), timings.back().startQuery);
    }

    vkCmdBeginRenderPass(commandBuffer[currentFrame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
    vkCmdExecuteCommands(commandBuffer[currentFrame], 1, &renderPassCommandBuffer);
    vkCmdEndRenderPass(commandBuffer[currentFrame]);

    if (vulkanRenderer->IsProfiling()) {
        vkCmdWriteTimestamp(commandBuffer[currentFrame], VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, vulkanRenderer->GetQueryPool(), timings.back().endQuery);
    }

    inRenderPass = false;
}

void VulkanCommandBuffer::BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) {
    assert(inRenderPass);
    assert(graphicsPipeline != nullptr);

    currentGraphicsPipeline = static_cast<VulkanGraphicsPipeline*>(graphicsPipeline);

    vkCmdBindPipeline(renderPassCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentGraphicsPipeline->GetPipeline(currentRenderPass));
}

void VulkanCommandBuffer::SetViewport(const glm::uvec2& origin, const glm::uvec2& size) {
    assert(inRenderPass);

    VkViewport viewport = {};
    viewport.x = static_cast<float>(origin.x);
    viewport.y = static_cast<float>(origin.x);
    viewport.width = static_cast<float>(size.x);
    viewport.height = static_cast<float>(size.y);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(renderPassCommandBuffer, 0, 1, &viewport);
}

void VulkanCommandBuffer::SetScissor(const glm::uvec2& origin, const glm::uvec2& size) {
    assert(inRenderPass);

    VkRect2D scissor = {};
    scissor.offset.x = origin.x;
    scissor.offset.y = origin.y;
    scissor.extent.width = size.x;
    scissor.extent.height = size.y;

    vkCmdSetScissor(renderPassCommandBuffer, 0, 1, &scissor);
}

void VulkanCommandBuffer::SetLineWidth(float width) {
    /// @todo Clamp line width to supported values.
    if (vulkanRenderer->IsWideLinesSupported()) {
        vkCmdSetLineWidth(renderPassCommandBuffer, width);
    }
}

void VulkanCommandBuffer::BindGeometry(const GeometryBinding* geometryBinding) {
    assert(inRenderPass);
    assert(geometryBinding != nullptr);

    const VulkanGeometryBinding* vulkanGeometryBinding = static_cast<const VulkanGeometryBinding*>(geometryBinding);

    const VkBuffer vertexBuffer = vulkanGeometryBinding->GetVertexBuffer();
    const VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(renderPassCommandBuffer, 0, 1, &vertexBuffer, &offset);

    GeometryBinding::IndexType indexType = vulkanGeometryBinding->GetIndexType();
    if (indexType != GeometryBinding::IndexType::NONE) {
        VkIndexType vkIndexType;

        switch (indexType) {
        case GeometryBinding::IndexType::SHORT:
            vkIndexType = VK_INDEX_TYPE_UINT16;
            break;
        case GeometryBinding::IndexType::INT:
            vkIndexType = VK_INDEX_TYPE_UINT32;
            break;
        default:
            Log(Log::ERR) << "Index type not supported.\n";
        }

        vkCmdBindIndexBuffer(renderPassCommandBuffer, vulkanGeometryBinding->GetIndexBuffer(), 0, vkIndexType);
    }
}

void VulkanCommandBuffer::BindUniformBuffer(ShaderProgram::BindingType bindingType, Buffer* uniformBuffer) {
    assert(inRenderPass);
    assert(currentGraphicsPipeline != nullptr);
    assert(uniformBuffer != nullptr && uniformBuffer->GetBufferUsage() == Buffer::BufferUsage::UNIFORM_BUFFER);
    assert(bindingType == ShaderProgram::BindingType::MATRICES || bindingType == ShaderProgram::BindingType::FRAGMENT_UNIFORMS);

    VkDescriptorSet descriptorSet = vulkanRenderer->GetDescriptorSet(bindingType, static_cast<VulkanBuffer*>(uniformBuffer));
    vkCmdBindDescriptorSets(renderPassCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentGraphicsPipeline->GetPipelineLayout(), bindingType, 1, &descriptorSet, 0, nullptr);
}

void VulkanCommandBuffer::BindStorageBuffer(Buffer* storageBuffer) {
    assert(inRenderPass);
    assert(currentGraphicsPipeline != nullptr);
    assert(storageBuffer != nullptr && storageBuffer->GetBufferUsage() == Buffer::BufferUsage::STORAGE_BUFFER);

    VkDescriptorSet descriptorSet = vulkanRenderer->GetDescriptorSet(ShaderProgram::BindingType::STORAGE_BUFFER, static_cast<VulkanBuffer*>(storageBuffer));
    vkCmdBindDescriptorSets(renderPassCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentGraphicsPipeline->GetPipelineLayout(), ShaderProgram::BindingType::STORAGE_BUFFER, 1, &descriptorSet, 0, nullptr);
}

void VulkanCommandBuffer::BindMaterial(std::initializer_list<const Texture*> textures) {
    assert(inRenderPass);

    for (const Texture* texture : textures) {
        assert(texture->GetType() != Texture::Type::RENDER_DEPTH);
        if (texture->GetType() == Texture::Type::RENDER_COLOR) {
            TransitionTexture(static_cast<const VulkanTexture*>(texture), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    VkDescriptorSet descriptorSet = vulkanRenderer->GetDescriptorSet(textures);
    vkCmdBindDescriptorSets(renderPassCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentGraphicsPipeline->GetPipelineLayout(), ShaderProgram::BindingType::MATERIAL, 1, &descriptorSet, 0, nullptr);
}

void VulkanCommandBuffer::PushConstants(const void* data) {
    assert(inRenderPass);
    assert(data != nullptr);

    const VkPushConstantRange* pushConstantRange = currentGraphicsPipeline->GetShaderProgram()->GetPushConstantRange();
    assert(pushConstantRange != nullptr);

    vkCmdPushConstants(renderPassCommandBuffer, currentGraphicsPipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL_GRAPHICS, pushConstantRange->offset, pushConstantRange->size, data);
}

void VulkanCommandBuffer::Draw(unsigned int vertexCount, unsigned int firstVertex) {
    assert(inRenderPass);

    vkCmdDraw(renderPassCommandBuffer, vertexCount, 1, firstVertex, 0);
}

void VulkanCommandBuffer::DrawIndexed(unsigned int indexCount, unsigned int firstIndex, unsigned int baseVertex) {
    assert(inRenderPass);

    vkCmdDrawIndexed(renderPassCommandBuffer, indexCount, 1, firstIndex, baseVertex, 0);
}

void VulkanCommandBuffer::BlitToSwapChain(Texture* texture) {
    assert(!inRenderPass);
    assert(texture != nullptr);

    // Transition swapchain image to transfer dst optimal.
    VkImage swapChainImage = vulkanRenderer->GetCurrentSwapChainImage();
    Utility::TransitionImage(commandBuffer[currentFrame], swapChainImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Blit from render texture to swapchain image.
    VulkanTexture* vulkanTexture = static_cast<VulkanTexture*>(texture);
    TransitionTexture(vulkanTexture, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    glm::uvec2 size = vulkanTexture->GetSize();

    VkImageBlit blit = {};
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = 0;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.srcOffsets[0].x = 0;
    blit.srcOffsets[0].y = 0;
    blit.srcOffsets[0].z = 0;
    blit.srcOffsets[1].x = size.x;
    blit.srcOffsets[1].y = size.y;
    blit.srcOffsets[1].z = 1;
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = 0;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;
    blit.dstOffsets[0].x = 0;
    blit.dstOffsets[0].y = 0;
    blit.dstOffsets[0].z = 0;
    blit.dstOffsets[1].x = size.x;
    blit.dstOffsets[1].y = size.y;
    blit.dstOffsets[1].z = 1;

    vkCmdBlitImage(commandBuffer[currentFrame], vulkanTexture->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_NEAREST);

    // Transition swapchain image to present optimal.
    Utility::TransitionImage(commandBuffer[currentFrame], swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

    containsBlitToSwapChain = true;
}

VkCommandBuffer VulkanCommandBuffer::GetCommandBuffer() const {
    return commandBuffer[currentFrame];
}

void VulkanCommandBuffer::End() {
    assert(!inRenderPass);

    ResetImageLayouts();
    timings.clear();

    if (!ended) {
        if (vkEndCommandBuffer(commandBuffer[currentFrame]) != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to end command buffer.\n";
        }

        ended = true;
    }
}

void VulkanCommandBuffer::NextFrame() {
    assert(ended);

    currentFrame = (currentFrame + 1) % swapChainImages;

    vkResetCommandBuffer(commandBuffer[currentFrame], 0);
    if (!secondaryCommandBuffers[currentFrame].empty()) {
        vkFreeCommandBuffers(device, commandPool, secondaryCommandBuffers[currentFrame].size(), secondaryCommandBuffers[currentFrame].data());
        secondaryCommandBuffers[currentFrame].clear();
    }

    containsBlitToSwapChain = false;
    renderTextureStates.clear();
    ended = false;

    Begin();
}

bool VulkanCommandBuffer::ContainsBlitToSwapChain() const {
    return containsBlitToSwapChain;
}

const std::vector<VulkanCommandBuffer::Timing>& VulkanCommandBuffer::GetTimings() const {
    return timings;
}

void VulkanCommandBuffer::Begin() {
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(commandBuffer[currentFrame], &beginInfo) != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to begin command buffer.\n";
    }
}

void VulkanCommandBuffer::TransitionTexture(const VulkanTexture* texture, VkImageLayout destinationImageLayout) {
    // Find the previous layout of the render texture.
    VkImageLayout oldLayout;
    auto it = renderTextureStates.find(texture);
    if (it != renderTextureStates.end()) {
        oldLayout = it->second;
    } else {
        // Assume the image is in the default layout for its type.
        switch (texture->GetType()) {
        case Texture::Type::RENDER_COLOR:
            oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            break;
        case Texture::Type::RENDER_DEPTH:
            oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            break;
        default:
            assert(0);
        }
    }
    
    // Transition to the desired layout (if it's not already in that layout).
    if (oldLayout != destinationImageLayout) {
        Utility::TransitionImage(commandBuffer[currentFrame], texture->GetImage(), oldLayout, destinationImageLayout);
        renderTextureStates[texture] = destinationImageLayout;
    }
}

void VulkanCommandBuffer::ResetImageLayouts() {
    // Transition all render textures back to their default layouts. 
    for (auto it : renderTextureStates) {
        VkImageLayout desiredLayout;
        switch (it.first->GetType()) {
        case Texture::Type::RENDER_COLOR:
            desiredLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            break;
        case Texture::Type::RENDER_DEPTH:
            desiredLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            break;
        default:
            assert(0);
        }

        if (it.second != desiredLayout) {
            TransitionTexture(it.first, desiredLayout);
        }
    }
}

} // namespace Video
