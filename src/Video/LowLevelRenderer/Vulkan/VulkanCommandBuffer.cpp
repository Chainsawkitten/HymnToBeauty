#include "VulkanCommandBuffer.hpp"

#include <Utility/Log.hpp>
#include "VulkanRenderer.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanComputePipeline.hpp"
#include "VulkanGeometryBinding.hpp"
#include "VulkanShader.hpp"
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
    currentComputePipeline = nullptr;
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
    currentGraphicsPipeline = nullptr;
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
    if (vulkanRenderer->GetOptionalFeatures().wideLines) {
        vkCmdSetLineWidth(renderPassCommandBuffer, width);
    }
}

void VulkanCommandBuffer::BindGeometry(GeometryBinding* geometryBinding) {
    assert(inRenderPass);
    assert(geometryBinding != nullptr);

    VulkanGeometryBinding* vulkanGeometryBinding = static_cast<VulkanGeometryBinding*>(geometryBinding);

    VulkanBuffer* vertexBuffer = vulkanGeometryBinding->GetVertexBuffer();
    const VkBuffer vulkanVertexBuffer = vertexBuffer->GetBuffer();

    // Barrier for vertex/storage buffers.
    if (vertexBuffer->GetBufferUsage() == Buffer::BufferUsage::VERTEX_STORAGE_BUFFER) {
        BufferBarrier(vertexBuffer, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, false);
    }

    const VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(renderPassCommandBuffer, 0, 1, &vulkanVertexBuffer, &offset);

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
    assert(uniformBuffer != nullptr && uniformBuffer->GetBufferUsage() == Buffer::BufferUsage::UNIFORM_BUFFER);
    assert(bindingType == ShaderProgram::BindingType::MATRICES || bindingType == ShaderProgram::BindingType::UNIFORMS);

    VkDescriptorSet descriptorSet = vulkanRenderer->GetDescriptorSet(bindingType, static_cast<VulkanBuffer*>(uniformBuffer));

    if (inRenderPass) {
        assert(currentGraphicsPipeline != nullptr);
        vkCmdBindDescriptorSets(renderPassCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentGraphicsPipeline->GetPipelineLayout(), bindingType, 1, &descriptorSet, 0, nullptr);
    } else {
        assert(currentComputePipeline != nullptr);
        vkCmdBindDescriptorSets(commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, currentComputePipeline->GetPipelineLayout(), bindingType, 1, &descriptorSet, 0, nullptr);
    }
}

void VulkanCommandBuffer::BindStorageBuffers(std::initializer_list<Buffer*> buffers) {
    const VulkanShaderProgram* shaderProgram = (inRenderPass ? currentGraphicsPipeline->GetShaderProgram() : currentComputePipeline->GetShaderProgram());

    // Synchronization.
    uint32_t index = 0;
    for (Buffer* buffer : buffers) {
        assert(buffer != nullptr);
        assert(buffer->GetBufferUsage() == Buffer::BufferUsage::STORAGE_BUFFER || buffer->GetBufferUsage() == Buffer::BufferUsage::VERTEX_STORAGE_BUFFER);

        const VulkanShaderProgram::StorageBufferInfo& storageBufferInfo = shaderProgram->GetStorageBufferInfo(index++);
        BufferBarrier(static_cast<VulkanBuffer*>(buffer), storageBufferInfo.pipelineStages, storageBufferInfo.readWrite);
    }

    // Bind descriptor set.
    VkDescriptorSet descriptorSet = vulkanRenderer->GetStorageBufferDescriptorSet(buffers);

    if (inRenderPass) {
        assert(currentGraphicsPipeline != nullptr);
        vkCmdBindDescriptorSets(renderPassCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentGraphicsPipeline->GetPipelineLayout(), ShaderProgram::BindingType::STORAGE_BUFFER, 1, &descriptorSet, 0, nullptr);
    } else {
        assert(currentComputePipeline != nullptr);
        vkCmdBindDescriptorSets(commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, currentComputePipeline->GetPipelineLayout(), ShaderProgram::BindingType::STORAGE_BUFFER, 1, &descriptorSet, 0, nullptr);
    }
}

void VulkanCommandBuffer::BindMaterial(std::initializer_list<Texture*> textures) {
    assert(inRenderPass);

    for (Texture* texture : textures) {
        assert(texture->GetType() != Texture::Type::RENDER_DEPTH);
        if (texture->GetType() == Texture::Type::RENDER_COLOR) {
            TransitionTexture(static_cast<VulkanTexture*>(texture), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    VkDescriptorSet descriptorSet = vulkanRenderer->GetDescriptorSet(textures);
    vkCmdBindDescriptorSets(renderPassCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentGraphicsPipeline->GetPipelineLayout(), ShaderProgram::BindingType::MATERIAL, 1, &descriptorSet, 0, nullptr);
}

void VulkanCommandBuffer::PushConstants(const void* data) {
    assert(data != nullptr);

    if (inRenderPass) {
        assert(currentGraphicsPipeline != nullptr);
        const VkPushConstantRange* pushConstantRange = currentGraphicsPipeline->GetShaderProgram()->GetPushConstantRange();
        assert(pushConstantRange != nullptr);

        vkCmdPushConstants(renderPassCommandBuffer, currentGraphicsPipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL_GRAPHICS, pushConstantRange->offset, pushConstantRange->size, data);
    } else {
        assert(currentComputePipeline != nullptr);
        const VkPushConstantRange* pushConstantRange = currentComputePipeline->GetShaderProgram()->GetPushConstantRange();
        assert(pushConstantRange != nullptr);

        vkCmdPushConstants(commandBuffer[currentFrame], currentComputePipeline->GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, pushConstantRange->offset, pushConstantRange->size, data);
    }
}

void VulkanCommandBuffer::Draw(unsigned int vertexCount, unsigned int firstVertex) {
    assert(inRenderPass);

    vkCmdDraw(renderPassCommandBuffer, vertexCount, 1, firstVertex, 0);
}

void VulkanCommandBuffer::DrawIndexed(unsigned int indexCount, unsigned int firstIndex, unsigned int baseVertex) {
    assert(inRenderPass);

    vkCmdDrawIndexed(renderPassCommandBuffer, indexCount, 1, firstIndex, baseVertex, 0);
}

void VulkanCommandBuffer::DrawIndexedInstanced(unsigned int indexCount, unsigned int instanceCount, unsigned int firstIndex, unsigned int baseVertex) {
    assert(inRenderPass);

    vkCmdDrawIndexed(renderPassCommandBuffer, indexCount, instanceCount, firstIndex, baseVertex, 0);
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

void VulkanCommandBuffer::BindComputePipeline(ComputePipeline* computePipeline) {
    assert(!inRenderPass);
    assert(computePipeline != nullptr);

    currentComputePipeline = static_cast<VulkanComputePipeline*>(computePipeline);

    vkCmdBindPipeline(commandBuffer[currentFrame], VK_PIPELINE_BIND_POINT_COMPUTE, currentComputePipeline->GetPipeline());
}

void VulkanCommandBuffer::Dispatch(const glm::uvec3& numGroups, const std::string& name) {
    assert(!inRenderPass);

    if (vulkanRenderer->IsProfiling()) {
        Timing timing;
        timing.name = name;
        timing.startQuery = vulkanRenderer->GetFreeQuery();
        timing.endQuery = vulkanRenderer->GetFreeQuery();

        timings.push_back(timing);

        vkCmdWriteTimestamp(commandBuffer[currentFrame], VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, vulkanRenderer->GetQueryPool(), timings.back().startQuery);
    }

    vkCmdDispatch(commandBuffer[currentFrame], numGroups.x, numGroups.y, numGroups.z);

    if (vulkanRenderer->IsProfiling()) {
        vkCmdWriteTimestamp(commandBuffer[currentFrame], VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, vulkanRenderer->GetQueryPool(), timings.back().endQuery);
    }
}

void VulkanCommandBuffer::ClearBuffer(Buffer* buffer) {
    assert(!inRenderPass);
    assert(buffer != nullptr);

    VulkanBuffer* vulkanBuffer = static_cast<VulkanBuffer*>(buffer);

    // Synchronization.
    if (vulkanBuffer->GetBufferUsage() == Buffer::BufferUsage::STORAGE_BUFFER || vulkanBuffer->GetBufferUsage() == Buffer::BufferUsage::VERTEX_STORAGE_BUFFER) {
        BufferBarrier(vulkanBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, true);
    }

    vkCmdFillBuffer(commandBuffer[currentFrame], vulkanBuffer->GetBuffer(), 0, VK_WHOLE_SIZE, 0);
}

VkCommandBuffer VulkanCommandBuffer::GetCommandBuffer() const {
    return commandBuffer[currentFrame];
}

void VulkanCommandBuffer::End() {
    assert(!inRenderPass);

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

    currentGraphicsPipeline = nullptr;
    currentComputePipeline = nullptr;
}

void VulkanCommandBuffer::TransitionTexture(VulkanTexture* texture, VkImageLayout destinationImageLayout) {
    // Find the previous layout of the render texture.
    VkImageLayout oldLayout = texture->GetImageLayout();
    
    // Transition to the desired layout (if it's not already in that layout).
    if (oldLayout != destinationImageLayout) {
        Utility::TransitionImage(commandBuffer[currentFrame], texture->GetImage(), oldLayout, destinationImageLayout);
        texture->SetImageLayout(destinationImageLayout);
    }
}

void VulkanCommandBuffer::BufferBarrier(VulkanBuffer* buffer, VkPipelineStageFlags stages, bool write) {
    VkPipelineStageFlags sourceStages = 0;
	VkPipelineStageFlags destinationStages = 0;
    VkPipelineStageFlags readStages = buffer->GetReadMask();
    VkAccessFlags sourceAccess = 0;
    VkAccessFlags destinationAccess = 0;

    // Read barrier.
    if (buffer->GetLastWriteStage() != 0 && (readStages & stages) != stages) {
        sourceStages |= buffer->GetLastWriteStage();
        if (sourceStages & (VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT))
            sourceAccess |= VK_ACCESS_SHADER_WRITE_BIT;
        if (sourceStages & VK_PIPELINE_STAGE_TRANSFER_BIT)
            sourceAccess |= VK_ACCESS_TRANSFER_WRITE_BIT;
        if (stages & (VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT))
            destinationAccess |= VK_ACCESS_SHADER_READ_BIT;
        if (stages & VK_PIPELINE_STAGE_VERTEX_INPUT_BIT)
            destinationAccess |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        buffer->SetReadMaskStage(stages);
		destinationStages |= stages & ~readStages;
    }

    // Write barrier.
    if (write) {
        sourceStages |= readStages;
        if (sourceStages & (VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT))
            sourceAccess |= VK_ACCESS_SHADER_READ_BIT;
        if (sourceStages & VK_PIPELINE_STAGE_VERTEX_INPUT_BIT)
            sourceAccess |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        if (stages & (VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT))
            destinationAccess |= VK_ACCESS_SHADER_WRITE_BIT;
        if (stages & VK_PIPELINE_STAGE_TRANSFER_BIT)
            destinationAccess |= VK_ACCESS_TRANSFER_WRITE_BIT;
        buffer->ClearReadMask();
        buffer->SetLastWriteStage(stages);
		destinationStages |= stages;
    }

    if (sourceStages != 0 && destinationStages != 0) {
        VkBufferMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        memoryBarrier.srcAccessMask = sourceAccess;
        memoryBarrier.dstAccessMask = destinationAccess;
        memoryBarrier.buffer = buffer->GetBuffer();
        memoryBarrier.size = VK_WHOLE_SIZE;

        vkCmdPipelineBarrier(commandBuffer[currentFrame], sourceStages, destinationStages, 0, 0, nullptr, 1, &memoryBarrier, 0, nullptr);
    }
}

} // namespace Video
