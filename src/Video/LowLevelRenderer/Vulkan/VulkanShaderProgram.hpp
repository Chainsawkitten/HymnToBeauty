#pragma once

#include "../Interface/ShaderProgram.hpp"

#include <initializer_list>
#include <vector>
#include <vulkan/vulkan.h>

namespace Video {

class Shader;
class VulkanShader;
class VulkanRenderer;

/// Vulkan implementation of ShaderProgram.
class VulkanShaderProgram : public ShaderProgram {
  public:
    /// Create new Vulkan shader program.
    /**
     * @param vulkanRenderer The Vulkan renderer.
     * @param device The Vulkan device.
     * @param shaders List of shaders to link together.
     */
    VulkanShaderProgram(VulkanRenderer* vulkanRenderer, std::initializer_list<const Shader*> shaders);

    /// Destructor.
    ~VulkanShaderProgram() final;

    /// Get the shaders in the shader program.
    /**
     * @return A list of shaders in the shader program.
     */
    const std::vector<const VulkanShader*>& GetShaders() const;

    /// Get the descriptor set layouts.
    /**
     * @return The descriptor set layouts.
     */
    const VkDescriptorSetLayout* GetDescriptorSetLayouts() const;

    /// Get the push constant range.
    /**
     * @return The push constant range, or nullptr if no push constants are used.
     */
    const VkPushConstantRange* GetPushConstantRange() const;

  private:
    VulkanShaderProgram(const VulkanShaderProgram& other) = delete;

    void AddUniformBuffer(BindingType bindingType);
    void AddStorageBuffer();
    void AddMaterial(unsigned int textures);
    void AddPushConstants(unsigned int pushConstantCount, ShaderSource::ReflectionInfo::PushConstant* pushConstants, bool isComputeShader);

    VulkanRenderer* vulkanRenderer;

    std::vector<const VulkanShader*> shaders;
    VkDescriptorSetLayout descriptorSetLayouts[ShaderProgram::BindingType::BINDING_TYPES];

    bool usesPushConstants = false;
    VkPushConstantRange pushConstantRange;
};

} // namespace Video
