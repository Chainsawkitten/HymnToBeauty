#pragma once

#include "../Interface/Shader.hpp"

#include "WebGPU.hpp"
#include <ShaderProcessor/ShaderSource.hpp>

namespace Video {

/// WebGPU implementation of Shader.
class WebGPUShader : public Shader {
  public:
    /// Create new WebGPU shader.
    /**
     * @param device WebGPU device.
     * @param shaderSource The source of the shader, generated by the shader processor.
     * @param type The type of shader to create.
     */
    WebGPUShader(WGPUDevice device, const ShaderSource& shaderSource, Type type);

    /// Destructor.
    ~WebGPUShader() final;

    /// Get reflection info.
    /**
     * @return The reflection info generated by the shader processor.
     */
    const ShaderSource::ReflectionInfo& GetReflectionInfo() const;

    /// Get the shader stage.
    /**
     * @return The shader stage.
     */
    WGPUShaderStage GetShaderStage() const;

    /// Get the shader module.
    /**
     * @return The shader module.
     */
    WGPUShaderModule GetShaderModule() const;

  private:
    WebGPUShader(const WebGPUShader& other) = delete;

    WGPUShaderModule shaderModule;
    ShaderSource::ReflectionInfo reflectionInfo;
    WGPUShaderStage shaderStage;
};

}