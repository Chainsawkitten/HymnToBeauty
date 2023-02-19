#pragma once

#include "../Interface/GraphicsPipeline.hpp"

#include <dawn/webgpu.h>
#include <map>

namespace Video {

class WebGPURenderer;
class ShaderProgram;
class WebGPUShaderProgram;
class VertexDescription;

/// WebGPU implementation of GraphicsPipeline.
class WebGPUGraphicsPipeline : public GraphicsPipeline {
  public:
    /// A description of a render pass.
    struct RenderPassDescription {
        /// Whether the render pass has a color attachment.
        bool hasColorAttachment;

        /// The format of the render pass' color attachment (if there is one).
        WGPUTextureFormat colorAttachmentFormat;

        /// Whether the render pass has a depth attachment.
        bool hasDepthAttachment;

        /// The format of the render pass' depth attachment (if there is one).
        WGPUTextureFormat depthAttachmentFormat;

        /// MSAA sample count when doing attachmentless rendering.
        uint32_t attachmentlessMsaa;
    };

    /// Create new WebGPU graphics pipeline.
    /**
     * @param renderer The WebGPU renderer.
     * @param shaderProgram The shader program to use.
     * @param configuration The configuration of the pipeline.
     * @param vertexDescription The description of the vertex input to the pipeline.
     */
    WebGPUGraphicsPipeline(WebGPURenderer& renderer, const ShaderProgram* shaderProgram, const Configuration& configuration, const VertexDescription* vertexDescription = nullptr);

    /// Destructor.
    ~WebGPUGraphicsPipeline() final;

    /// Get the WebGPU pipeline for a given render pass.
    /**
     * @param renderPassDescription A description of the render pass to generate a pipeline for.
     * 
     * @return The internal WebGPU pipeline.
     */
    WGPURenderPipeline GetRenderPipeline(const RenderPassDescription& renderPassDescription);

    /// Get the shader program.
    /**
     * @return The shader program.
     */
    const WebGPUShaderProgram* GetShaderProgram() const;

  private:
    WebGPUGraphicsPipeline(const WebGPUGraphicsPipeline& other) = delete;

    const WebGPUShaderProgram* webGPUShaderProgram;

    WGPUDevice device;
    WGPUPipelineLayout pipelineLayout;
    WGPUVertexBufferLayout vertexBufferLayout;
    WGPURenderPipelineDescriptor pipelineDescriptor;
    WGPUPrimitiveDepthClipControl depthClipControl;
    WGPUDepthStencilState depthStencilState;
    WGPUFragmentState fragmentState;
    WGPUColorTargetState colorTargetState;
    WGPUBlendState blendState;

    // Pipeline cache.
    struct RenderPassCompare {
        bool operator()(const RenderPassDescription& a, const RenderPassDescription& b) const {
            if (a.hasColorAttachment != b.hasColorAttachment) {
                return a.hasColorAttachment < b.hasColorAttachment;
            } else if (a.hasColorAttachment && b.hasColorAttachment) {
                if (a.colorAttachmentFormat != b.colorAttachmentFormat) {
                    return a.colorAttachmentFormat < b.colorAttachmentFormat;
                }
            }

            if (a.hasDepthAttachment != b.hasDepthAttachment) {
                return a.hasDepthAttachment < b.hasDepthAttachment;
            } else if (a.hasDepthAttachment && b.hasDepthAttachment) {
                if (a.depthAttachmentFormat != b.depthAttachmentFormat) {
                    return a.depthAttachmentFormat < b.depthAttachmentFormat;
                }
            }

            if (a.attachmentlessMsaa != b.attachmentlessMsaa) {
                return a.attachmentlessMsaa < b.attachmentlessMsaa;
            }

            return false;
        }
    };

    std::map<RenderPassDescription, WGPURenderPipeline, RenderPassCompare> pipelines;
};

}
