#include "WebGPUShaderProgram.hpp"

#include <cassert>
#include "WebGPUShader.hpp"

namespace Video {

WebGPUShaderProgram::WebGPUShaderProgram(WGPUDevice device, std::initializer_list<const Shader*> shaders) {
    for (const Shader* shader : shaders) {
        const WebGPUShader* webGPUShader = static_cast<const WebGPUShader*>(shader);
        this->shaders.push_back(webGPUShader);
    }

    // All bind group layouts begin empty (until bindings are added).
    for (unsigned int i = 0; i < BindingType::BINDING_TYPES; ++i) {
        WGPUBindGroupLayoutDescriptor emptyBindGroupLayout = {};
        bindGroupLayouts[i] = wgpuDeviceCreateBindGroupLayout(device, &emptyBindGroupLayout);
        bindGroupLayoutIsEmpty[i] = true;
    }

    std::vector<StorageBufferInfo> storageBufferInfos;
    for (const Shader* shader : shaders) {
        const WebGPUShader* webGPUShader = static_cast<const WebGPUShader*>(shader);

        ShaderSource::ReflectionInfo reflectionInfo = webGPUShader->GetReflectionInfo();
        if (reflectionInfo.hasMatrices) {
            AddMatrices(device);
        }

        if (reflectionInfo.hasUniforms) {
            hasUniformsBuffer = true;
        }

        for (uint32_t i = 0; i < reflectionInfo.storageBufferCount; i++) {
            uint32_t binding = reflectionInfo.storageBuffers[i].binding;

            if (binding >= storageBufferInfos.size()) {
                StorageBufferInfo clear = {0, false};
                storageBufferInfos.resize(binding + 1, clear);
            }

            storageBufferInfos[binding].shaderStages |= webGPUShader->GetShaderStage();
            storageBufferInfos[binding].readWrite = storageBufferInfos[binding].readWrite || reflectionInfo.storageBuffers[i].readWrite;
        }

        if (reflectionInfo.materialCount > 0) {
            AddMaterial(device, reflectionInfo.materialCount);
        }

        // Emulate push constants.
        if (reflectionInfo.pushConstantCount > 0) {
            AddPushConstants(device, reflectionInfo.pushConstantCount, reflectionInfo.pushConstants);
        }
    }

    if (hasUniformsBuffer || pushConstantSize > 0) {
        AddUniforms(device, hasUniformsBuffer);
    }
    AddStorageBuffers(device, storageBufferInfos);
}

WebGPUShaderProgram::~WebGPUShaderProgram() {
    for (unsigned int i = 0; i < BindingType::BINDING_TYPES; ++i) {
        wgpuBindGroupLayoutRelease(bindGroupLayouts[i]);
    }
}

const std::vector<const WebGPUShader*>& WebGPUShaderProgram::GetShaders() const {
    return shaders;
}

const WGPUBindGroupLayout* WebGPUShaderProgram::GetBindGroupLayouts() const {
    return bindGroupLayouts;
}

bool WebGPUShaderProgram::IsBindGroupLayoutEmpty(ShaderProgram::BindingType bindingType) const {
    return bindGroupLayoutIsEmpty[bindingType];
}

bool WebGPUShaderProgram::HasUniformsBuffer() const {
    return hasUniformsBuffer;
}

uint32_t WebGPUShaderProgram::GetPushConstantSize() const {
    return pushConstantSize;
}

void WebGPUShaderProgram::AddMatrices(WGPUDevice device) {
    // Release previous (empty) bind group layout.
    wgpuBindGroupLayoutRelease(bindGroupLayouts[BindingType::MATRICES]);

    // Create new bind group layout.
    WGPUBindGroupLayoutEntry entry = {};
    entry.binding = 0;
    entry.buffer.type = WGPUBufferBindingType_Uniform;
    entry.buffer.hasDynamicOffset = false;
    entry.buffer.minBindingSize = 0;
    entry.visibility = WGPUShaderStage_Vertex;

    WGPUBindGroupLayoutDescriptor bindGroupLayout = {};
    bindGroupLayout.entryCount = 1;
    bindGroupLayout.entries = &entry;

    bindGroupLayouts[BindingType::MATRICES] = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayout);
    bindGroupLayoutIsEmpty[BindingType::MATRICES] = false;
}

void WebGPUShaderProgram::AddStorageBuffers(WGPUDevice device, const std::vector<StorageBufferInfo>& storageBufferInfos) {
    if (storageBufferInfos.empty()) {
        return;
    }

    // Release previous (empty) bind group layout.
    wgpuBindGroupLayoutRelease(bindGroupLayouts[BindingType::STORAGE_BUFFER]);

    // Create new bind group layout.
    WGPUBindGroupLayoutEntry* entries = new WGPUBindGroupLayoutEntry[storageBufferInfos.size()];
    for (uint32_t i = 0; i < storageBufferInfos.size(); ++i) {
        entries[i] = {};
        entries[i].binding = i;
        entries[i].visibility = storageBufferInfos[i].shaderStages;
        entries[i].buffer.type = (storageBufferInfos[i].readWrite ? WGPUBufferBindingType_Storage : WGPUBufferBindingType_ReadOnlyStorage);
        entries[i].buffer.hasDynamicOffset = false;
        entries[i].buffer.minBindingSize = 0;
    }

    WGPUBindGroupLayoutDescriptor bindGroupLayout = {};
    bindGroupLayout.entryCount = storageBufferInfos.size();
    bindGroupLayout.entries = entries;

    bindGroupLayouts[BindingType::STORAGE_BUFFER] = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayout);
    bindGroupLayoutIsEmpty[BindingType::STORAGE_BUFFER] = false;

    delete[] entries;
}

void WebGPUShaderProgram::AddMaterial(WGPUDevice device, unsigned int count) {
    if (count == 0) {
        return;
    }

    // Release previous (empty) bind group layout.
    wgpuBindGroupLayoutRelease(bindGroupLayouts[BindingType::MATERIAL]);

    // Create new bind group layout.
    WGPUBindGroupLayoutEntry* entries = new WGPUBindGroupLayoutEntry[count * 2];
    for (uint32_t i = 0; i < count; ++i) {
        // Texture
        entries[i] = {};
        entries[i].binding = i;
        entries[i].visibility = WGPUShaderStage_Fragment;
        entries[i].texture.sampleType = WGPUTextureSampleType_Float;
        entries[i].texture.viewDimension = WGPUTextureViewDimension_2D;

        // Sampler
        entries[i + count] = {};
        entries[i + count].binding = i + count;
        entries[i + count].visibility = WGPUShaderStage_Fragment;
        entries[i + count].sampler.type = WGPUSamplerBindingType_Filtering;
    }

    WGPUBindGroupLayoutDescriptor bindGroupLayout = {};
    bindGroupLayout.entryCount = count * 2;
    bindGroupLayout.entries = entries;

    bindGroupLayouts[BindingType::MATERIAL] = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayout);
    bindGroupLayoutIsEmpty[BindingType::MATERIAL] = false;

    delete[] entries;
}

void WebGPUShaderProgram::AddPushConstants(WGPUDevice device, unsigned int pushConstantCount, ShaderSource::ReflectionInfo::PushConstant* pushConstants) {
    for (unsigned int i = 0; i < pushConstantCount; ++i) {
        pushConstantSize += PushConstantTypeToSize(pushConstants[i].type);
    }
}

void WebGPUShaderProgram::AddUniforms(WGPUDevice device, bool hasUniformsBuffer) {
    assert(hasUniformsBuffer || pushConstantSize > 0);

    // Release previous (empty) bind group layout.
    wgpuBindGroupLayoutRelease(bindGroupLayouts[BindingType::UNIFORMS]);

    // Create new bind group layout.
    WGPUBindGroupLayoutDescriptor bindGroupLayout = {};
    bindGroupLayout.entryCount = 0;

    WGPUBindGroupLayoutEntry entries[2];
    if (hasUniformsBuffer) {
        entries[bindGroupLayout.entryCount] = {};
        entries[bindGroupLayout.entryCount].binding = 0;
        entries[bindGroupLayout.entryCount].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
        entries[bindGroupLayout.entryCount].buffer.type = WGPUBufferBindingType_Uniform;
        entries[bindGroupLayout.entryCount].buffer.hasDynamicOffset = false;
        entries[bindGroupLayout.entryCount].buffer.minBindingSize = 0;

        ++bindGroupLayout.entryCount;
    }

    if (pushConstantSize > 0) {
        entries[bindGroupLayout.entryCount] = {};
        entries[bindGroupLayout.entryCount].binding = 1;
        entries[bindGroupLayout.entryCount].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
        entries[bindGroupLayout.entryCount].buffer.type = WGPUBufferBindingType_Uniform;
        entries[bindGroupLayout.entryCount].buffer.hasDynamicOffset = false;
        entries[bindGroupLayout.entryCount].buffer.minBindingSize = 0;

        ++bindGroupLayout.entryCount;
    }

    bindGroupLayout.entries = entries;

    bindGroupLayouts[BindingType::UNIFORMS] = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayout);
    bindGroupLayoutIsEmpty[BindingType::UNIFORMS] = false;
}

}
