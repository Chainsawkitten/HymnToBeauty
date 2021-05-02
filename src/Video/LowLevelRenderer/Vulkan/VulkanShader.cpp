#include "VulkanShader.hpp"

#include <Utility/Log.hpp>
#include <assert.h>
#include <ShaderProcessor/ShaderSource.hpp>
#include <fstream>
#include <vector>
#include <iostream>

namespace Video {

VulkanShader::VulkanShader(VkDevice device, const ShaderSource& shaderSource, Type type) {
    this->device = device;
    this->reflectionInfo = shaderSource.reflectionInfo;

    // Convert type of shader to Vulkan shader stage.
    switch (type) {
    case Type::VERTEX_SHADER:
        shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
        break;
    case Type::FRAGMENT_SHADER:
        shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
        break;
    case Type::COMPUTE_SHADER:
        shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
        break;
    default:
        assert(false);
    }

    // Create shader module.
    VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
    shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleCreateInfo.codeSize = shaderSource.spirvLength;
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderSource.spirvSource);

    vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule);
}

VulkanShader::~VulkanShader() {
    vkDestroyShaderModule(device, shaderModule, nullptr);
}

VkShaderModule VulkanShader::GetShaderModule() const {
    return shaderModule;
}

VkShaderStageFlagBits VulkanShader::GetShaderStage() const {
    return shaderStage;
}

const ShaderSource::ReflectionInfo& VulkanShader::GetReflectionInfo() const {
    return reflectionInfo;
}

} // namespace Video
