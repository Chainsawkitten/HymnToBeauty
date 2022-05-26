#pragma once

#include "../Interface/CommandBuffer.hpp"
#include <vulkan/vulkan.h>
#include <vector>

namespace Video {

class VulkanBuffer;
class VulkanRenderer;
class VulkanRenderPass;
class VulkanRenderPassAllocator;
class VulkanGraphicsPipeline;
class VulkanComputePipeline;
class VulkanTexture;

/// Vulkan implementation of CommandBuffer.
class VulkanCommandBuffer : public CommandBuffer {
  public:
    /// Timing for a block of work.
    struct Timing {
        /// Name of the block of work.
        std::string name;

        /// The timestamp at the start of the block of work.
        uint32_t startQuery;

        /// The timestamp at the end of the block of work.
        uint32_t endQuery;
    };

    /// Create new Vulkan command buffer.
    /**
     * @param vulkanRenderer The Vulkan renderer.
     * @param device The Vulkan device.
     * @param commandPool The command pool to allocate the command buffer from.
     * @param renderPassAllocator The render pass allocator.
     */
    VulkanCommandBuffer(VulkanRenderer* vulkanRenderer, VkDevice device, VkCommandPool commandPool, VulkanRenderPassAllocator& renderPassAllocator);

    /// Destructor.
    ~VulkanCommandBuffer() final;

    void BeginRenderPass(RenderPass* renderPass, const std::string& name) final;
    void BeginRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation, const std::string& name) final;
    void BeginAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples, const std::string& name) final;
    void EndRenderPass() final;
    void BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) final;
    void SetViewport(const glm::uvec2& origin, const glm::uvec2& size) final;
    void SetScissor(const glm::uvec2& origin, const glm::uvec2& size) final;
    void SetLineWidth(float width) final;
    void BindGeometry(GeometryBinding* geometryBinding) final;
    void BindUniformBuffer(ShaderProgram::BindingType bindingType, Buffer* uniformBuffer) final;
    void BindStorageBuffers(std::initializer_list<Buffer*> buffers) final;
    void BindMaterial(std::initializer_list<std::pair<Texture*, const Sampler*>> textures) final;
    void PushConstants(const void* data) final;
    void Draw(unsigned int vertexCount, unsigned int firstVertex) final;
    void DrawIndexed(unsigned int indexCount, unsigned int firstIndex, unsigned int baseVertex) final;
    void DrawIndexedInstanced(unsigned int indexCount, unsigned int instanceCount, unsigned int firstIndex, unsigned int baseVertex) final;
    void BlitToSwapChain(Texture* texture) final;
    void BindComputePipeline(ComputePipeline* computePipeline) final;
    void Dispatch(const glm::uvec3& numGroups, const std::string& name) final;
    void ClearBuffer(Buffer* buffer) final;

    /// Get command buffer.
    /**
     * @return The internal Vulkan command buffer.
     */
    VkCommandBuffer GetCommandBuffer() const;

    /// Finish recording the command buffer.
    void End();

    /// Cycle the command buffer for the next frame.
    void NextFrame();

    /// Get whether the command buffer contains a blit to the swap chain.
    /**
     * @return Whether the command buffer contains a blit to the swap chain.
     */
    bool ContainsBlitToSwapChain() const;

    /// Get the all timings in the command buffer.
    /**
     * @return All recorded timings.
     */
    const std::vector<Timing>& GetTimings() const;

  private:
    VulkanCommandBuffer(const VulkanCommandBuffer& other) = delete;

    void Begin();

    void TransitionTexture(VulkanTexture* texture, VkImageLayout destinationImageLayout);
    void BufferBarrier(VulkanBuffer* buffer, VkPipelineStageFlags stages, bool write);

    VulkanRenderer* vulkanRenderer;
    VkDevice device;
    VkCommandPool commandPool;
    VulkanRenderPassAllocator& renderPassAllocator;

    VkCommandBuffer* commandBuffer;
    VkCommandBuffer renderPassCommandBuffer;
    std::vector<VkCommandBuffer>* secondaryCommandBuffers;
    uint32_t currentFrame = 0;
    uint32_t swapChainImages;

    bool ended = false;
    bool inRenderPass = false;
    bool containsBlitToSwapChain = false;

    VkClearValue clearValues[2];
    VkRenderPassBeginInfo renderPassBeginInfo;

    const VulkanRenderPass* currentRenderPass = nullptr;
    VulkanGraphicsPipeline* currentGraphicsPipeline = nullptr;
    VulkanComputePipeline* currentComputePipeline = nullptr;

    std::vector<Timing> timings;
};

} // namespace Video
