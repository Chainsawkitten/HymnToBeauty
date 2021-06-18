#include "VulkanShaderProgram.hpp"

#include <assert.h>
#include <Utility/Log.hpp>
#include "VulkanShader.hpp"
#include "VulkanRenderer.hpp"

namespace Video {

VulkanShaderProgram::VulkanShaderProgram(VulkanRenderer* vulkanRenderer, std::initializer_list<const Shader*> shaders) {
    assert(vulkanRenderer != nullptr);

    this->vulkanRenderer = vulkanRenderer;

    // All descriptor set layouts begin empty (until bindings are added).
    for (unsigned int i = 0; i < BindingType::BINDING_TYPES; ++i) {
        descriptorSetLayouts[i] = vulkanRenderer->GetEmptyDescriptorSetLayout();
    }

    for (const Shader* shader : shaders) {
        const VulkanShader* vulkanShader = static_cast<const VulkanShader*>(shader);
        this->shaders.push_back(vulkanShader);

        // Resource info.
        ShaderSource::ReflectionInfo reflectionInfo = vulkanShader->GetReflectionInfo();
        if (reflectionInfo.hasMatrices) {
            AddUniformBuffer(BindingType::MATRICES);
        }

        if (reflectionInfo.hasUniforms) {
            AddUniformBuffer(BindingType::UNIFORMS);
        }

        if (reflectionInfo.storageBufferCount > 0) {
            for (uint32_t i = 0; i < reflectionInfo.storageBufferCount; i++) {
                uint32_t binding = reflectionInfo.storageBuffers[i].binding;

                if (binding >= storageBufferInfo.size()) {
                    StorageBufferInfo clear = { 0, false };
                    storageBufferInfo.resize(binding + 1, clear);
                }

                switch (vulkanShader->GetShaderStage()) {
                case VK_SHADER_STAGE_VERTEX_BIT:
                    storageBufferInfo[binding].pipelineStages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
                    break;
                case VK_SHADER_STAGE_FRAGMENT_BIT:
                    storageBufferInfo[binding].pipelineStages |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                    break;
                case VK_SHADER_STAGE_COMPUTE_BIT:
                    storageBufferInfo[binding].pipelineStages |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                    break;
                }
                storageBufferInfo[binding].readWrite = storageBufferInfo[binding].readWrite || reflectionInfo.storageBuffers[i].readWrite;
            }
        }

        if (reflectionInfo.materialCount > 0) {
            AddMaterial(reflectionInfo.materialCount);
        }

        if (reflectionInfo.pushConstantCount > 0) {
            AddPushConstants(reflectionInfo.pushConstantCount, reflectionInfo.pushConstants, vulkanShader->GetShaderStage() == VK_SHADER_STAGE_COMPUTE_BIT);
        }
    }

    if (storageBufferInfo.size() > 0)
        descriptorSetLayouts[BindingType::STORAGE_BUFFER] = vulkanRenderer->GetStorageBufferDescriptorSetLayout(storageBufferInfo.size());
}

VulkanShaderProgram::~VulkanShaderProgram() {

}

const std::vector<const VulkanShader*>& VulkanShaderProgram::GetShaders() const {
    return shaders;
}

const VkDescriptorSetLayout* VulkanShaderProgram::GetDescriptorSetLayouts() const {
    return descriptorSetLayouts;
}

const VkPushConstantRange* VulkanShaderProgram::GetPushConstantRange() const {
    return usesPushConstants ? &pushConstantRange : nullptr;
}

const VulkanShaderProgram::StorageBufferInfo& VulkanShaderProgram::GetStorageBufferInfo(uint32_t binding) const {
    assert(binding < storageBufferInfo.size());

    return storageBufferInfo[binding];
}

void VulkanShaderProgram::AddUniformBuffer(BindingType bindingType) {
    assert(bindingType == BindingType::MATRICES || bindingType == BindingType::UNIFORMS);

    /// @todo Check that we're not adding things twice.

    descriptorSetLayouts[bindingType] = vulkanRenderer->GetBufferDescriptorSetLayout(bindingType);
}

void VulkanShaderProgram::AddMaterial(unsigned int textures) {
    /// @todo Check that we're not adding things twice.

    descriptorSetLayouts[BindingType::MATERIAL] = vulkanRenderer->GetMaterialDescriptorSetLayout(textures);
}

void VulkanShaderProgram::AddPushConstants(unsigned int pushConstantCount, ShaderSource::ReflectionInfo::PushConstant* pushConstants, bool isComputeShader) {
    assert(pushConstantCount > 0 && pushConstants != nullptr);
    assert(!usesPushConstants);

    pushConstantRange.stageFlags = (isComputeShader ? VK_SHADER_STAGE_COMPUTE_BIT : VK_SHADER_STAGE_ALL_GRAPHICS);
    pushConstantRange.offset = 0;
    pushConstantRange.size = 0;

    for (unsigned int i = 0; i < pushConstantCount; ++i) {
        pushConstantRange.size += PushConstantTypeToSize(pushConstants[i].type);
    }

    assert(pushConstantRange.size % 4 == 0);

    usesPushConstants = true;
}

}
