#include "WebGPUComputePipeline.hpp"

#include <vector>
#include <assert.h>
#include <Utility/Log.hpp>

#include "WebGPUShader.hpp"
#include "WebGPUShaderProgram.hpp"

namespace Video {

WebGPUComputePipeline::WebGPUComputePipeline(WGPUDevice device, const ShaderProgram* shaderProgram) {
    assert(shaderProgram != nullptr);

    webGPUShaderProgram = static_cast<const WebGPUShaderProgram*>(shaderProgram);

    // Pipeline layout.
    WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
    pipelineLayoutDescriptor.bindGroupLayoutCount = ShaderProgram::BindingType::BINDING_TYPES;
    pipelineLayoutDescriptor.bindGroupLayouts = webGPUShaderProgram->GetBindGroupLayouts();
    pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);

    WGPUComputePipelineDescriptor pipelineDescriptor = {};
    pipelineDescriptor.layout = pipelineLayout;

    // Shader stages.
    const std::vector<const WebGPUShader*>& shaders = webGPUShaderProgram->GetShaders();
    for (const WebGPUShader* shader : shaders) {
        switch (shader->GetShaderStage()) {
        case WGPUShaderStage_Compute:
            pipelineDescriptor.compute.module = shader->GetShaderModule();
            pipelineDescriptor.compute.entryPoint = "main";
            break;
        default:
            assert(false);
        }
    }

    pipeline = wgpuDeviceCreateComputePipeline(device, &pipelineDescriptor);
}

WebGPUComputePipeline::~WebGPUComputePipeline() {
    wgpuComputePipelineRelease(pipeline);
    wgpuPipelineLayoutRelease(pipelineLayout);
}

WGPUComputePipeline WebGPUComputePipeline::GetComputePipeline() const {
    return pipeline;
}

const WebGPUShaderProgram* WebGPUComputePipeline::GetShaderProgram() const {
    return webGPUShaderProgram;
}

}
