#include "WebGPUShader.hpp"

#include <cassert>
#include <ShaderProcessor/ShaderSource.hpp>

namespace Video {

WebGPUShader::WebGPUShader(WGPUDevice device, const ShaderSource& shaderSource, Type type) {
    assert(shaderSource.webGPUSpirvLength > 0);
    assert(shaderSource.webGPUSpirvSource != nullptr);

    this->reflectionInfo = shaderSource.reflectionInfo;

    // Create shader module.
    WGPUShaderModuleSPIRVDescriptor spirvDescriptor = {};
    spirvDescriptor.chain.sType = WGPUSType_ShaderModuleSPIRVDescriptor;
    spirvDescriptor.codeSize = shaderSource.webGPUSpirvLength / sizeof(uint32_t);
    spirvDescriptor.code = reinterpret_cast<const uint32_t*>(shaderSource.webGPUSpirvSource);

    WGPUShaderModuleDescriptor shaderModuleDescriptor = {};
    shaderModuleDescriptor.nextInChain = reinterpret_cast<const WGPUChainedStruct*>(&spirvDescriptor);

    shaderModule = wgpuDeviceCreateShaderModule(device, &shaderModuleDescriptor);

    // Get shader stage.
    switch (type) {
    case Type::VERTEX_SHADER:
        shaderStage = WGPUShaderStage_Vertex;
        break;
    case Type::FRAGMENT_SHADER:
        shaderStage = WGPUShaderStage_Fragment;
        break;
    case Type::COMPUTE_SHADER:
        shaderStage = WGPUShaderStage_Compute;
        break;
    default:
        assert(false);
    }
}

WebGPUShader::~WebGPUShader() {
    wgpuShaderModuleRelease(shaderModule);
}

const ShaderSource::ReflectionInfo& WebGPUShader::GetReflectionInfo() const {
    return reflectionInfo;
}

WGPUShaderStage WebGPUShader::GetShaderStage() const {
    return shaderStage;
}

WGPUShaderModule WebGPUShader::GetShaderModule() const {
    return shaderModule;
}

} // namespace Video
