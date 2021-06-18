#include "VulkanGraphicsPipeline.hpp"

#include "VulkanRenderer.hpp"
#include "VulkanShader.hpp"
#include "VulkanShaderProgram.hpp"
#include "VulkanVertexDescription.hpp"

#include <assert.h>
#include <Utility/Log.hpp>

namespace Video {

VulkanGraphicsPipeline::VulkanGraphicsPipeline(const VulkanRenderer& renderer, VkDevice device, const ShaderProgram* shaderProgram, const Configuration& configuration, const VertexDescription* vertexDescription) {
    assert(shaderProgram != nullptr);

    this->device = device;
    vulkanShaderProgram = static_cast<const VulkanShaderProgram*>(shaderProgram);

    // Create pipeline layout.
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

    // Shader stages.
    const std::vector<const VulkanShader*>& shaders = vulkanShaderProgram->GetShaders();
    for (const VulkanShader* shader : shaders) {
        VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
        shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo.stage = shader->GetShaderStage();
        shaderStageCreateInfo.module = shader->GetShaderModule();
        shaderStageCreateInfo.pName = "main";

        shaderStages.push_back(shaderStageCreateInfo);
    }

    // Vertex input.
    vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (vertexDescription != nullptr) {
        const VulkanVertexDescription* vulkanVertexDescription = static_cast<const VulkanVertexDescription*>(vertexDescription);

        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = vulkanVertexDescription->GetBindingDescription();
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vulkanVertexDescription->GetAttributeCount();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = vulkanVertexDescription->GetAttributeDescriptions();
    } else {
        vertexInputStateCreateInfo.vertexBindingDescriptionCount = 0;
        vertexInputStateCreateInfo.pVertexBindingDescriptions = VK_NULL_HANDLE;
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 0;
        vertexInputStateCreateInfo.pVertexAttributeDescriptions = VK_NULL_HANDLE;
    }

    // Input assembly.
    inputAssemblyCreateInfo = {};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    switch (configuration.primitiveType) {
    case PrimitiveType::POINT:
        inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        break;
    case PrimitiveType::LINE:
        inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        break;
    case PrimitiveType::TRIANGLE:
        inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        break;
    }

    // Viewport.
    viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.pViewports = VK_NULL_HANDLE;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pScissors = VK_NULL_HANDLE;

    // Rasterizer.
    rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = configuration.depthClamp;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = 1.0f;

    switch (configuration.polygonMode) {
    case PolygonMode::FILL:
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
        break;
    case PolygonMode::LINE:
        rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
        break;
    }

    switch (configuration.cullFace) {
    case CullFace::NONE:
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
        break;
    case CullFace::BACK:
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        break;
    case CullFace::FRONT:
        rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
        break;
    }

    // Conservative rasterization.
    conservativeRasterizationInfo = {};
    if (configuration.conservativeRasterization) {
        conservativeRasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT;
        conservativeRasterizationInfo.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;

        rasterizationStateCreateInfo.pNext = &conservativeRasterizationInfo;
    }

    // Multisampling.
    multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

    // Depth stencil.
    depthStencilState = {};
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilState.depthBoundsTestEnable = VK_FALSE;
    depthStencilState.stencilTestEnable = VK_FALSE;

    switch (configuration.depthMode) {
    case DepthMode::NONE:
        depthStencilState.depthTestEnable = VK_FALSE;
        depthStencilState.depthWriteEnable = VK_FALSE;
        break;
    case DepthMode::TEST:
        depthStencilState.depthTestEnable = VK_TRUE;
        depthStencilState.depthWriteEnable = VK_FALSE;
        break;
    case DepthMode::TEST_WRITE:
        depthStencilState.depthTestEnable = VK_TRUE;
        depthStencilState.depthWriteEnable = VK_TRUE;
        break;
    }

    switch (configuration.depthComparison) {
    case DepthComparison::LESS:
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
        break;
    case DepthComparison::LESS_EQUAL:
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        break;
    case DepthComparison::ALWAYS:
        depthStencilState.depthCompareOp = VK_COMPARE_OP_ALWAYS;
        break;
    }

    // Color blending.
    blendAttachmentState = {};
    blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    switch (configuration.blendMode) {
    case BlendMode::NONE:
        blendAttachmentState.blendEnable = VK_FALSE;
        break;
    case BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA:
        blendAttachmentState.blendEnable = VK_TRUE;
        blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        break;
    }

    blendStateCreateInfo = {};
    blendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blendStateCreateInfo.logicOpEnable = VK_FALSE;
    blendStateCreateInfo.attachmentCount = 1;
    blendStateCreateInfo.pAttachments = &blendAttachmentState;

    // Dynamic state.
    uint32_t dynamicStateCount = 0;
    dynamicState[dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicState[dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
    if ((configuration.primitiveType == PrimitiveType::LINE || configuration.polygonMode == PolygonMode::LINE) && renderer.GetOptionalFeatures().wideLines) {
        dynamicState[dynamicStateCount++] = VK_DYNAMIC_STATE_LINE_WIDTH;
    }

    dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
    dynamicStateCreateInfo.pDynamicStates = dynamicState;

    // Create graphics pipeline.
    pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = shaderStages.size();
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pColorBlendState = &blendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.subpass = 0;
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline() {
    for (auto it : pipelines) {
        vkDestroyPipeline(device, it.second, nullptr);
    }

    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
}

VkPipeline VulkanGraphicsPipeline::GetPipeline(const VulkanRenderPass* renderPass) {
    const VulkanRenderPass::Compatibility compatibility = renderPass->GetCompatiblity();

    auto it = pipelines.find(compatibility);
    if (it != pipelines.end()) {
        // Use compatible pipeline.
        return it->second;
    } else {
        // Create pipeline.
        multisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(compatibility.attachmentlessMsaa);
        pipelineCreateInfo.renderPass = renderPass->GetRenderPass();

        VkPipeline pipeline;
        VkResult result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
        if (result != VK_SUCCESS) {
            Log(Log::ERR) << "Failed to create graphics pipeline.\n";
        }

        pipelines[compatibility] = pipeline;
        return pipeline;
    }
}

VkPipelineLayout VulkanGraphicsPipeline::GetPipelineLayout() const {
    return pipelineLayout;
}

const VulkanShaderProgram* VulkanGraphicsPipeline::GetShaderProgram() const {
    return vulkanShaderProgram;
}

}
