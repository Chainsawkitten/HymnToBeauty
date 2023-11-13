#pragma once

#include "../Interface/CommandBuffer.hpp"

#include "WebGPU.hpp"
#include <vector>
#include "WebGPUGraphicsPipeline.hpp"

namespace Video {

class WebGPURenderer;
class WebGPUBuffer;
class WebGPUComputePipeline;
class WebGPUTexture;

/// WebGPU implementation of CommandBuffer.
class WebGPUCommandBuffer : public CommandBuffer {
  public:
    /// Create new WebGPU command buffer.
    /**
     * @param renderer The WebGPU renderer.
     * @param frames How many frames before re-using buffers for push constant emulation.
     */
    WebGPUCommandBuffer(WebGPURenderer* renderer, uint32_t frames = 1);

    /// Destructor.
    ~WebGPUCommandBuffer() final;

    void BeginRenderPass(RenderPass* renderPass, const std::string& name) final;
    void BeginRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation, const std::string& name) final;
    void BeginAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples, const std::string& name) final;
    void EndRenderPass() final;
    void BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) final;
    void SetViewport(const glm::uvec2& origin, const glm::uvec2& size) final;
    void SetScissor(const glm::uvec2& origin, const glm::uvec2& size) final;
    void SetLineWidth(float width) final;
    void BindGeometry(GeometryBinding* geometryBinding) final;
    void SetIndexBuffer(Buffer* indexBuffer, GeometryBinding::IndexType format, uint64_t offset) final;
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

    /// Get command encoder.
    /**
     * @return The internal WebGPU command encoder.
     */
    WGPUCommandEncoder GetCommandEncoder();

    /// Finish recording the command buffer.
    /**
     * @return The resuing WGPUCommandBuffer that can be submitted to a queue.
     */
    WGPUCommandBuffer End();

    /// Cycle the command buffer for the next frame.
    void NextFrame();

    /// Get the current render pass encoder.
    /**
     * @return The current render pass encoder.
     */
    WGPURenderPassEncoder GetRenderPassEncoder();

  private:
    WebGPUCommandBuffer(const WebGPUCommandBuffer& other) = delete;

    void AllocatePushConstantBuffer();
    void UpdateUniforms();
    void EndComputePass();
    const WebGPUShaderProgram* GetCurrentShaderProgram() const;

    WebGPURenderer* renderer;
    WGPUDevice device;
    WGPUCommandEncoder commandEncoder = nullptr;
    WGPURenderPassEncoder renderPassEncoder;
    WGPUComputePassEncoder computePassEncoder;

    WebGPUGraphicsPipeline::RenderPassDescription renderPassDescription;
    WGPUBindGroup emptyBindGroup;

    WebGPUGraphicsPipeline* currentGraphicsPipeline = nullptr;
    WebGPUComputePipeline* currentComputePipeline = nullptr;

    GraphicsPipeline* blitGraphicsPipeline;
    const Sampler* blitSampler;

    bool inRenderPass = false;
    bool inComputePass = false;

    // minUniformBufferOffsetAlignment is guaranteed to be at most 256.
    static const uint32_t pushConstantSize = 256;
    static const uint32_t pushConstantsPerBuffer = 1024;

    struct PushConstantBuffer {
        WebGPUBuffer* buffer;
        uint8_t data[pushConstantSize * pushConstantsPerBuffer];
        uint32_t currentPushConstant;
    };

    uint32_t frames;
    uint32_t frame = 0;
    std::vector<PushConstantBuffer>* pushConstantBuffers;
    uint32_t currentPushConstantBuffer = 0;


    bool uniformsHasChanged = false;
    const WebGPUBuffer* currentUniformBuffer;

    WebGPUTexture* dummyRenderTarget = nullptr;

    std::vector<WGPUBindGroup> bindGroupsToRelease;
};

} // namespace Video
