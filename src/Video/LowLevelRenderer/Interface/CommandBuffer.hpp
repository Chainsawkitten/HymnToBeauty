#pragma once

#include <glm/glm.hpp>
#include "Types.hpp"
#include "ShaderProgram.hpp"
#include <initializer_list>

namespace Video {

class VertexDescription;
class GeometryBinding;
class Texture;
class Buffer;
class RenderPass;
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
     */
    virtual void BeginRenderPass(RenderPass* renderPass, const std::string& name = "Untitled render pass") = 0;

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
    virtual void BindMaterial(std::initializer_list<Texture*> textures) = 0;

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
     */
    virtual void Dispatch(const glm::uvec3& numGroups) = 0;

  private:
    CommandBuffer(const CommandBuffer& other) = delete;
};

}
