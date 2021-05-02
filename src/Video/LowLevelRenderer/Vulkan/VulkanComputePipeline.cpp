#include "VulkanComputePipeline.hpp"

#include <vector>
#include <assert.h>
#include <Utility/Log.hpp>

#include "VulkanShader.hpp"
#include "VulkanShaderProgram.hpp"

namespace Video {

VulkanComputePipeline::VulkanComputePipeline(VkDevice device, const ShaderProgram* shaderProgram) {
    assert(shaderProgram != nullptr);

    this->device = device;
    vulkanShaderProgram = static_cast<const VulkanShaderProgram*>(shaderProgram);

    // Pipeline layout.
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    const VkDescriptorSetLayout* descriptorSetLayouts = vulkanShaderProgram->GetDescriptorSetLayouts();
    pipelineLayoutCreateInfo.setLayoutCount = ShaderProgram::BindingType::BINDING_TYPES;
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts;

    const VkPushConstantRange* pushConstantRange = vulkanShaderProgram->GetPushConstantRange();
    if (pushConstantRange != nullptr) {
        pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
        pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRange;
    }

    VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
    if (result != VK_SUCCESS) {
        Log(Log::ERR) << "Failed to create pipeline layout.\n";
    }

    // Create info.
    VkComputePipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = pipelineLayout;

    // Shader stage.
    const std::vector<const VulkanShader*>& shaders = vulkanShaderProgram->GetShaders();
    for (const VulkanShader* shader : shaders) {
        assert(shader->GetShaderStage() == VK_SHADER_STAGE_COMPUTE_BIT);
        
        pipelineCreateInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        pipelineCreateInfo.stage.stage = shader->GetShaderStage();
        pipelineCreateInfo.stage.module = shader->GetShaderModule();
        pipelineCreateInfo.stage.pName = "main";
    }

    // Create compute pipeline.
    vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
}

VulkanComputePipeline::~VulkanComputePipeline() {
    vkDestroyPipeline(device, pipeline, nullptr);
    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

VkPipeline VulkanComputePipeline::GetPipeline() const {
    return pipeline;
}

VkPipelineLayout VulkanComputePipeline::GetPipelineLayout() const {
    return pipelineLayout;
}

const VulkanShaderProgram* VulkanComputePipeline::GetShaderProgram() const {
    return vulkanShaderProgram;
}

}
