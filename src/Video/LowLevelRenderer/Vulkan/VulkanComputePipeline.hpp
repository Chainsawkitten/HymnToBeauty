#pragma once

#include "../Interface/ComputePipeline.hpp"

#include <vulkan/vulkan.h>

namespace Video {

class ShaderProgram;
class VulkanShaderProgram;

/// Vulkan implementation of ComputePipeline.
class VulkanComputePipeline : public ComputePipeline {
  public:
    /// Create new Vulkan compute pipeline.
    /**
     * @param device The Vulkan device.
     * @param shaderProgram The shader program to use.
     */
    explicit VulkanComputePipeline(VkDevice device, const ShaderProgram* shaderProgram);

    /// Destructor.
    ~VulkanComputePipeline() final;

    /// Get the Vulkan pipeline.
    /**
     * @return The internal Vulkan pipeline.
     */
    VkPipeline GetPipeline() const;

    /// Get the Vulkan pipeline layout.
    /**
     * @return The pipeline layout.
     */
    VkPipelineLayout GetPipelineLayout() const;

    /// Get the Vulkan shader program.
    /**
     * @return The shader program.
     */
    const VulkanShaderProgram* GetShaderProgram() const;

  private:
    VulkanComputePipeline(const VulkanComputePipeline& other) = delete;

    VkDevice device;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    const VulkanShaderProgram* vulkanShaderProgram;
};

}
