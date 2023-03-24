#pragma once

#include "../Interface/ShaderProgram.hpp"

#include <initializer_list>
#include <vector>
#include "WebGPU.hpp"

namespace Video {

class Shader;
class WebGPUShader;
class WebGPURenderer;

/// WebGPU implementation of ShaderProgram.
class WebGPUShaderProgram : public ShaderProgram {
  public:
    /// Create new WebGPU shader program.
    /**
     * @param device The WebGPU device.
     * @param shaders List of shaders to link together.
     */
    WebGPUShaderProgram(WGPUDevice device, std::initializer_list<const Shader*> shaders);

    /// Destructor.
    ~WebGPUShaderProgram() final;

    /// Get the shaders in the shader program.
    /**
     * @return A list of shaders in the shader program.
     */
    const std::vector<const WebGPUShader*>& GetShaders() const;

    /// Get the bind group layouts.
    /**
     * @return The bind group layouts.
     */
    const WGPUBindGroupLayout* GetBindGroupLayouts() const;

    /// Get whether a bind group layout is empty.
    /**
     * @param bindingType The binding type to check the bind group layout for.
     * 
     * @return Whether the bind group layout is empty.
     */
    bool IsBindGroupLayoutEmpty(ShaderProgram::BindingType bindingType) const;

    /// Get whether the UNIFORMS bind group has a uniform buffer.
    /**
     * @return Whether the shader program has a uniform buffer.
     */
    bool HasUniformsBuffer() const;

    /// Get the size of the push constants.
    /**
     * @return The size of the push constants.
     */
    uint32_t GetPushConstantSize() const;

  private:
    WebGPUShaderProgram(const WebGPUShaderProgram& other) = delete;

    struct StorageBufferInfo {
        WGPUShaderStageFlags shaderStages;
        bool readWrite;
    };

    void AddMatrices(WGPUDevice device);
    void AddStorageBuffers(WGPUDevice device, const std::vector<StorageBufferInfo>& storageBufferInfos);
    void AddMaterial(WGPUDevice device, unsigned int count);
    void AddPushConstants(WGPUDevice device, unsigned int pushConstantCount, ShaderSource::ReflectionInfo::PushConstant* pushConstants);
    void AddUniforms(WGPUDevice device, bool hasUniformsBuffer);

    std::vector<const WebGPUShader*> shaders;
    // And extra bind group for emulated push constants.
    WGPUBindGroupLayout bindGroupLayouts[ShaderProgram::BindingType::BINDING_TYPES];
    bool bindGroupLayoutIsEmpty[ShaderProgram::BindingType::BINDING_TYPES];

    bool hasUniformsBuffer = false;
    uint32_t pushConstantSize = 0;
};

} // namespace Video
