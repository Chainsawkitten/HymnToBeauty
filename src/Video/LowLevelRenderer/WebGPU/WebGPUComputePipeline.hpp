#pragma once

#include "../Interface/ComputePipeline.hpp"

#include "WebGPU.hpp"

namespace Video {

class ShaderProgram;
class WebGPUShaderProgram;

/// WebGPU implementation of ComputePipeline.
class WebGPUComputePipeline : public ComputePipeline {
  public:
    /// Create new WebGPU compute pipeline.
    /**
     * @param device The WebGPU device.
     * @param shaderProgram The shader program to use.
     */
    explicit WebGPUComputePipeline(WGPUDevice device, const ShaderProgram* shaderProgram);

    /// Destructor.
    ~WebGPUComputePipeline() final;

    /// Get the WebGPU pipeline.
    /**
     * @return The internal WebGPU pipeline.
     */
    WGPUComputePipeline GetComputePipeline() const;

    /// Get the Vulkan shader program.
    /**
     * @return The shader program.
     */
    const WebGPUShaderProgram* GetShaderProgram() const;

  private:
    WebGPUComputePipeline(const WebGPUComputePipeline& other) = delete;

    const WebGPUShaderProgram* webGPUShaderProgram;

    WGPUPipelineLayout pipelineLayout;
    WGPUComputePipeline pipeline;
};

}
