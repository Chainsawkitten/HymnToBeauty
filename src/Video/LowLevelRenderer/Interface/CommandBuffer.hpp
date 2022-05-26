#pragma once

#include <glm/glm.hpp>
#include "Types.hpp"
#include "ShaderProgram.hpp"
#include "RenderPass.hpp"
#include <initializer_list>
#include <utility>

namespace Video {

class VertexDescription;
class GeometryBinding;
class Texture;
class Sampler;
class Buffer;
class GraphicsPipeline;
class ComputePipeline;

/// A buffer into which rendering commands are recorded.
/**
 * Command buffers have to be recorded in the same order as they are submitted.
 */
class CommandBuffer {
  public:
    /// Create a new command buffer.
    CommandBuffer() {}

    /// Destructor.
    virtual ~CommandBuffer() {}

    /// Begin render pass.
    /**
     * @param renderPass The render pass to begin.
     * @param name The name of the render pass (displayed in profiling).
     */
    virtual void BeginRenderPass(RenderPass* renderPass, const std::string& name = "Untitled render pass") = 0;

    /// Begin render pass.
    /**
     * @param colorAttachment The color attachment to draw to.
     * @param colorLoadOperation What to do with the previous contents of the color attachment.
     * @param depthAttachment The depth attachment to draw to.
     * @param depthLoadOperation What to do with the previous contents of the depth attachment.
     * @param name The name of the render pass (displayed in profiling).
     */
    virtual void BeginRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation = RenderPass::LoadOperation::CLEAR, Texture* depthAttachment = nullptr, RenderPass::LoadOperation depthLoadOperation = RenderPass::LoadOperation::CLEAR, const std::string& name = "Untitled render pass") = 0;

    /// Begin attachmentless render pass.
    /**
     * @param size The framebuffer size.
     * @param msaaSamples Number of MSAA samples.
     * @param name The name of the render pass (displayed in profiling).
     */
    virtual void BeginAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples = 1, const std::string& name = "Untitled render pass") = 0;

    /// End render pass.
    virtual void EndRenderPass() = 0;

    /// Bind graphics pipeline.
    /**
     * @param graphicsPipeline The graphics pipeline to bind.
     */
    virtual void BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) = 0;

    /// Set the viewport to render to.
    /**
     * @param origin The bottom-left corner of the viewport.
     * @param size The size of the viewport.
     */
    virtual void SetViewport(const glm::uvec2& origin, const glm::uvec2& size) = 0;

    /// Set the scissor box.
    /**
     * @param origin The bottom-left corner of the scissor box.
     * @param size The size of the scissor box.
     */
    virtual void SetScissor(const glm::uvec2& origin, const glm::uvec2& size) = 0;

    /// Set both viewport and scissor.
    /**
     * @param origin The bottom-left corner of the viewport/scissor box.
     * @param size The size of the viewport/scissor box.
     */
    void SetViewportAndScissor(const glm::uvec2& origin, const glm::uvec2& size) {
        SetViewport(origin, size);
        SetScissor(origin, size);
    }

    /// Set width of lines.
    /**
     * @param width The line width.
     */
    virtual void SetLineWidth(float width) = 0;

    /// Bind geometry to be used in upcoming draw calls.
    /**
     * @param geometryBinding The geometry to bind.
     */
    virtual void BindGeometry(GeometryBinding* geometryBinding) = 0;

    /// Bind uniform buffer for use in a shader.
    /**
     * @param bindingType Where to bind the buffer.
     * @param uniformBuffer The buffer to bind.
     */
    virtual void BindUniformBuffer(ShaderProgram::BindingType bindingType, Buffer* uniformBuffer) = 0;

    /// Bind storage buffers.
    /**
     * @param buffers The buffers to bind.
     */
    virtual void BindStorageBuffers(std::initializer_list<Buffer*> buffers) = 0;

    /// Bind a material.
    /**
     * @param textures The textures to bind to the graphics pipeline.
     */
    virtual void BindMaterial(std::initializer_list<std::pair<Texture*, const Sampler*>> textures) = 0;

    /// Update push constants.
    /**
     * @param data The data to push to the shader.
     */
    virtual void PushConstants(const void* data) = 0;

    /// Draw geometry.
    /**
     * @param vertexCount The number of vertices to render.
     * @param firstVertex The vertex to start rendering at.
     */
    virtual void Draw(unsigned int vertexCount, unsigned int firstVertex = 0) = 0;

    /// Draw indexed geometry.
    /**
     * @param indexCount The number of indices to render.
     * @param firstIndex The index to start rendering at.
     * @param baseVertex Constant that should be added to each value in the index buffer.
     */
    virtual void DrawIndexed(unsigned int indexCount, unsigned int firstIndex = 0, unsigned int baseVertex = 0) = 0;

    /// Draw indexed instanced geometry.
    /**
     * @param indexCount The number of indices to render.
     * @param instanceCount The number of instances to render.
     * @param firstIndex The index to start rendering at.
     * @param baseVertex Constant that should be added to each value in the index buffer.
     */
    virtual void DrawIndexedInstanced(unsigned int indexCount, unsigned int instanceCount = 1, unsigned int firstIndex = 0, unsigned int baseVertex = 0) = 0;

    /// Blit a texture to the current swap chain image.
    /**
     * @param texture The texture to copy to the swap chain.
     */
    virtual void BlitToSwapChain(Texture* texture) = 0;

    /// Bind compute pipeline.
    /**
     * @param computePipeline The compute pipeline to bind.
     */
    virtual void BindComputePipeline(ComputePipeline* computePipeline) = 0;

    /// Dispatch compute shader.
    /**
     * @param numGroups The number of groups in each dimension.
     * @param name The name of the dispatch (displayed in profiling).
     */
    virtual void Dispatch(const glm::uvec3& numGroups, const std::string& name = "Untitled dispatch") = 0;

    /// Clear a buffer (fill with 0).
    /**
     * @param buffer The buffer to clear.
     */
    virtual void ClearBuffer(Buffer* buffer) = 0;

  private:
    CommandBuffer(const CommandBuffer& other) = delete;
};

}
