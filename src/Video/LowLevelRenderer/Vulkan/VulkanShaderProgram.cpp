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
        this->shaders.push_back(static_cast<const VulkanShader*>(shader));

        ShaderSource::ReflectionInfo reflectionInfo = static_cast<const VulkanShader*>(shader)->GetReflectionInfo();
        if (reflectionInfo.hasMatrices) {
            AddUniformBuffer(BindingType::MATRICES);
        }

        if (reflectionInfo.hasFragmentUniforms) {
            AddUniformBuffer(BindingType::FRAGMENT_UNIFORMS);
        }

        if (reflectionInfo.hasStorageBuffer) {
            AddStorageBuffer();
        }

        if (reflectionInfo.materialCount > 0) {
            AddMaterial(reflectionInfo.materialCount);
        }

        if (reflectionInfo.pushConstantCount > 0) {
            AddPushConstants(reflectionInfo.pushConstantCount, reflectionInfo.pushConstants);
        }
    }
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

void VulkanShaderProgram::AddUniformBuffer(BindingType bindingType) {
    assert(bindingType == BindingType::MATRICES || bindingType == BindingType::FRAGMENT_UNIFORMS);

    /// @todo Check that we're not adding things twice.

    descriptorSetLayouts[bindingType] = vulkanRenderer->GetBufferDescriptorSetLayout(bindingType);
}

void VulkanShaderProgram::AddStorageBuffer() {
    descriptorSetLayouts[BindingType::STORAGE_BUFFER] = vulkanRenderer->GetBufferDescriptorSetLayout(BindingType::STORAGE_BUFFER);
}

void VulkanShaderProgram::AddMaterial(unsigned int textures) {
    /// @todo Check that we're not adding things twice.

    descriptorSetLayouts[BindingType::MATERIAL] = vulkanRenderer->GetMaterialDescriptorSetLayout(textures);
}

void VulkanShaderProgram::AddPushConstants(unsigned int pushConstantCount, ShaderSource::ReflectionInfo::PushConstant* pushConstants) {
    assert(pushConstantCount > 0 && pushConstants != nullptr);
    assert(!usesPushConstants);

    pushConstantRange.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
    pushConstantRange.offset = 0;
    pushConstantRange.size = 0;

    for (unsigned int i = 0; i < pushConstantCount; ++i) {
        pushConstantRange.size += PushConstantTypeToSize(pushConstants[i].type);
    }

    assert(pushConstantRange.size % 4 == 0);

    usesPushConstants = true;
}

}
