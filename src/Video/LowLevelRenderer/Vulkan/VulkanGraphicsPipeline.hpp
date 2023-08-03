#pragma once

#include "../Interface/GraphicsPipeline.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanTexture.hpp"

#include <glad/vulkan.h>
#include <vector>
#include <map>

namespace Video {

class VulkanRenderer;
class ShaderProgram;
class VulkanShaderProgram;
class VertexDescription;

/// Vulkan implementation of GraphicsPipeline.
class VulkanGraphicsPipeline : public GraphicsPipeline {
  public:
    /// Create new Vulkan graphics pipeline.
    /**
     * @param renderer The Vulkan renderer.
     * @param device The Vulkan device.
     * @param shaderProgram The shader program to use.
     * @param configuration The configuration of the pipeline.
     * @param vertexDescription The description of the vertex input to the pipeline.
     */
    VulkanGraphicsPipeline(const VulkanRenderer& renderer, VkDevice device, const ShaderProgram* shaderProgram, const Configuration& configuration, const VertexDescription* vertexDescription = nullptr);

    /// Destructor.
    ~VulkanGraphicsPipeline() final;

    /// Get the Vulkan pipeline for a given render pass.
    /**
     * @param renderPass The RenderPass to be compatible with.
     *
     * @return The internal Vulkan pipeline.
     */
    VkPipeline GetPipeline(const VulkanRenderPass* renderPass);

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
    VulkanGraphicsPipeline(const VulkanGraphicsPipeline& other) = delete;

    VkDevice device;
    VkPipelineLayout pipelineLayout;

    const VulkanShaderProgram* vulkanShaderProgram;

    // Pipeline creation info.
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo;
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    VkPipelineRasterizationConservativeStateCreateInfoEXT conservativeRasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    VkPipelineColorBlendAttachmentState blendAttachmentState;
    VkPipelineColorBlendStateCreateInfo blendStateCreateInfo;
    VkDynamicState dynamicState[3];
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    VkGraphicsPipelineCreateInfo pipelineCreateInfo;

    // Pipeline cache.
    struct RenderPassCompare {
        bool operator()(const VulkanRenderPass::Compatibility& a, const VulkanRenderPass::Compatibility& b) const {
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

    std::map<VulkanRenderPass::Compatibility, VkPipeline, RenderPassCompare> pipelines;
};

}
