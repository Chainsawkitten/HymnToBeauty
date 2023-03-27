#include "WebGPUCommandBuffer.hpp"

#include "WebGPURenderer.hpp"
#include "WebGPUTexture.hpp"
#include "WebGPUSampler.hpp"
#include "WebGPUBuffer.hpp"
#include "WebGPUShaderProgram.hpp"
#include "WebGPUGeometryBinding.hpp"
#include "WebGPUComputePipeline.hpp"

#include <Utility/Log.hpp>
#include <cstring>
#include <cassert>

namespace Video {

WebGPUCommandBuffer::WebGPUCommandBuffer(WebGPURenderer* renderer) {
    this->renderer = renderer;
    device = renderer->GetDevice();
    blitGraphicsPipeline = renderer->GetBlitGraphicsPipeline();
    blitSampler = renderer->GetSampler(Sampler::Filter::NEAREST, Sampler::Clamping::CLAMP_TO_EDGE);

    NextFrame();

    // Create empty bind group to bind for all empty bind group layouts.
    WGPUBindGroupLayoutDescriptor emptyBindGroupLayoutDesc = {};
    WGPUBindGroupLayout emptyBindGroupLayout = wgpuDeviceCreateBindGroupLayout(device, &emptyBindGroupLayoutDesc);

    WGPUBindGroupDescriptor emptyBindGroupDesc = {};
    emptyBindGroupDesc.layout = emptyBindGroupLayout;

    emptyBindGroup = wgpuDeviceCreateBindGroup(device, &emptyBindGroupDesc);
    wgpuBindGroupLayoutRelease(emptyBindGroupLayout);
}

WebGPUCommandBuffer::~WebGPUCommandBuffer() {
    if (commandEncoder != nullptr) {
        wgpuCommandEncoderRelease(commandEncoder);
    }

    wgpuBindGroupRelease(emptyBindGroup);

    for (PushConstantBuffer& buffer : pushConstantBuffers) {
        delete buffer.buffer;
    }
}

void WebGPUCommandBuffer::BeginRenderPass(RenderPass* renderPass, const std::string& name) {
    Log(Log::ERR) << "WebGPUCommandBuffer::BeginRenderPass should not be called.\n";
    assert(false);
}

void WebGPUCommandBuffer::BeginRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation, const std::string& name) {
    assert(!inRenderPass);
    assert(colorAttachment != nullptr || depthAttachment != nullptr);

    EndComputePass();

    WGPURenderPassColorAttachment passColorAttachment = {};
    WGPURenderPassDepthStencilAttachment passDepthAttachment = {};

    WGPURenderPassDescriptor renderPassDescriptor = {};
    renderPassDescriptor.label = name.c_str();
    renderPassDescription = {};

    if (colorAttachment != nullptr) {
        passColorAttachment.view = static_cast<WebGPUTexture*>(colorAttachment)->GetTextureView();
        passColorAttachment.clearValue.r = 0.0;
        passColorAttachment.clearValue.g = 0.0;
        passColorAttachment.clearValue.b = 0.0;
        passColorAttachment.clearValue.a = 0.0;
        switch (colorLoadOperation) {
        case RenderPass::LoadOperation::LOAD:
            passColorAttachment.loadOp = WGPULoadOp_Load;
            break;
        case RenderPass::LoadOperation::CLEAR:
        case RenderPass::LoadOperation::DONT_CARE:
            passColorAttachment.loadOp = WGPULoadOp_Clear;
            break;
        }
        passColorAttachment.storeOp = WGPUStoreOp_Store;

        renderPassDescriptor.colorAttachmentCount = 1;
        renderPassDescriptor.colorAttachments = &passColorAttachment;

        renderPassDescription.hasColorAttachment = true;
        renderPassDescription.colorAttachmentFormat = static_cast<WebGPUTexture*>(colorAttachment)->GetTextureFormat();
    }

    if (depthAttachment != nullptr) {
        passDepthAttachment.view = static_cast<WebGPUTexture*>(depthAttachment)->GetTextureView();
        passDepthAttachment.depthClearValue = 1.0f;
        switch (depthLoadOperation) {
        case RenderPass::LoadOperation::LOAD:
            passDepthAttachment.depthLoadOp = WGPULoadOp_Load;
            break;
        case RenderPass::LoadOperation::CLEAR:
        case RenderPass::LoadOperation::DONT_CARE:
            passDepthAttachment.depthLoadOp = WGPULoadOp_Clear;
            break;
        }
        passDepthAttachment.depthStoreOp = WGPUStoreOp_Store;
        passDepthAttachment.stencilLoadOp = WGPULoadOp_Clear;
        passDepthAttachment.stencilStoreOp = WGPUStoreOp_Discard;

        renderPassDescriptor.depthStencilAttachment = &passDepthAttachment;

        renderPassDescription.hasDepthAttachment = true;
        renderPassDescription.depthAttachmentFormat = static_cast<WebGPUTexture*>(depthAttachment)->GetTextureFormat();
    }

    renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);

    inRenderPass = true;
}

void WebGPUCommandBuffer::BeginAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples, const std::string& name) {
    assert(!inRenderPass);

    EndComputePass();

    // WebGPU doesn't support attachmentless rendering, so create a dummy render target.
    dummyRenderTarget = renderer->CreateAttachmentlessRenderTarget(size, msaaSamples);

    WGPURenderPassColorAttachment passColorAttachment = {};
    WGPURenderPassDepthStencilAttachment passDepthAttachment = {};

    WGPURenderPassDescriptor renderPassDescriptor = {};
    renderPassDescriptor.label = name.c_str();

    passColorAttachment.view = static_cast<WebGPUTexture*>(dummyRenderTarget)->GetTextureView();
    passColorAttachment.clearValue.r = 0.0;
    passColorAttachment.clearValue.g = 0.0;
    passColorAttachment.clearValue.b = 0.0;
    passColorAttachment.clearValue.a = 0.0;
    passColorAttachment.loadOp = WGPULoadOp_Clear;
    passColorAttachment.storeOp = WGPUStoreOp_Discard;

    renderPassDescriptor.colorAttachmentCount = 1;
    renderPassDescriptor.colorAttachments = &passColorAttachment;

    renderPassDescription = {};
    renderPassDescription.attachmentlessMsaa = msaaSamples;

    renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);

    inRenderPass = true;
}

void WebGPUCommandBuffer::EndRenderPass() {
    assert(inRenderPass);
    
    wgpuRenderPassEncoderEnd(renderPassEncoder);
#if WEBGPU_BACKEND_DAWN
    wgpuRenderPassEncoderRelease(renderPassEncoder);
#endif

    // Free attachmentless render target.
    if (dummyRenderTarget != nullptr) {
        renderer->FreeAttachmentlessRenderTarget(dummyRenderTarget);
        dummyRenderTarget = nullptr;
    }

    inRenderPass = false;
}

void WebGPUCommandBuffer::BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) {
    assert(inRenderPass);
    assert(graphicsPipeline != nullptr);

    // Get render pipeline.
    currentGraphicsPipeline = static_cast<WebGPUGraphicsPipeline*>(graphicsPipeline);
    WGPURenderPipeline renderPipeline = currentGraphicsPipeline->GetRenderPipeline(renderPassDescription);

    wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipeline);

    // Bind empty bind groups for all empty bind group layouts.
    for (uint32_t binding = 0; binding < ShaderProgram::BindingType::BINDING_TYPES; ++binding) {
        if (currentGraphicsPipeline->GetShaderProgram()->IsBindGroupLayoutEmpty(static_cast<ShaderProgram::BindingType>(binding))) {
            wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, binding, emptyBindGroup, 0, nullptr);
        }
    }

    uniformsHasChanged = false;
}

void WebGPUCommandBuffer::SetViewport(const glm::uvec2& origin, const glm::uvec2& size) {
    assert(inRenderPass);

    wgpuRenderPassEncoderSetViewport(renderPassEncoder, static_cast<float>(origin.x), static_cast<float>(origin.y), static_cast<float>(size.x), static_cast<float>(size.y), 0.0f, 1.0f);
}

void WebGPUCommandBuffer::SetScissor(const glm::uvec2& origin, const glm::uvec2& size) {
    assert(inRenderPass);
    
    wgpuRenderPassEncoderSetScissorRect(renderPassEncoder, origin.x, origin.y, size.x, size.y);
}

void WebGPUCommandBuffer::SetLineWidth(float width) {
    assert(inRenderPass);
    
    // WebGPU doesn't support wide lines. Do nothing.
}

void WebGPUCommandBuffer::BindGeometry(GeometryBinding* geometryBinding) {
    assert(inRenderPass);
    assert(geometryBinding != nullptr);

    WebGPUGeometryBinding* webGPUGeometryBinding = static_cast<WebGPUGeometryBinding*>(geometryBinding);
    wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, webGPUGeometryBinding->GetVertexBuffer()->GetBuffer(), 0, webGPUGeometryBinding->GetVertexBuffer()->GetSize());

    WGPUIndexFormat indexFormat;
    switch (webGPUGeometryBinding->GetIndexType()) {
    case GeometryBinding::IndexType::NONE:
        indexFormat = WGPUIndexFormat_Undefined;
        break;
    case GeometryBinding::IndexType::SHORT:
        indexFormat = WGPUIndexFormat_Uint16;
        break;
    case GeometryBinding::IndexType::INT:
        indexFormat = WGPUIndexFormat_Uint32;
        break;
    }
    if (indexFormat != WGPUIndexFormat_Undefined) {
        wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, webGPUGeometryBinding->GetIndexBuffer()->GetBuffer(), indexFormat, 0, webGPUGeometryBinding->GetIndexBuffer()->GetSize());
    }
}

void WebGPUCommandBuffer::BindUniformBuffer(ShaderProgram::BindingType bindingType, Buffer* uniformBuffer) {
    assert(uniformBuffer != nullptr && uniformBuffer->GetBufferUsage() == Buffer::BufferUsage::UNIFORM_BUFFER);
    assert(bindingType == ShaderProgram::BindingType::MATRICES || bindingType == ShaderProgram::BindingType::UNIFORMS);

    if (bindingType == ShaderProgram::BindingType::UNIFORMS && GetCurrentShaderProgram()->GetPushConstantSize() > 0) {
        // Delay setting the bind group until before the draw.
        // This is because we may not yet know the full state of the bind group because of emulated push constants.
        currentUniformBuffer = static_cast<const WebGPUBuffer*>(uniformBuffer);
        uniformsHasChanged = true;
        return;
    }

    // Create and set bind group.
    WGPUBindGroupEntry entry = {};
    entry.binding = 0;
    entry.buffer = static_cast<const WebGPUBuffer*>(uniformBuffer)->GetBuffer();
    entry.size = uniformBuffer->GetSize();

    WGPUBindGroupDescriptor bindGroupDescriptor = {};
    bindGroupDescriptor.layout = currentGraphicsPipeline->GetShaderProgram()->GetBindGroupLayouts()[bindingType];
    bindGroupDescriptor.entryCount = 1;
    bindGroupDescriptor.entries = &entry;
    WGPUBindGroup uniformBindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);

    if (inRenderPass) {
        wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, bindingType, uniformBindGroup, 0, nullptr);
    } else {
        assert(bindingType != ShaderProgram::BindingType::MATRICES);
        wgpuComputePassEncoderSetBindGroup(computePassEncoder, bindingType, uniformBindGroup, 0, nullptr);
    }
    bindGroupsToRelease.push_back(uniformBindGroup);
}

void WebGPUCommandBuffer::BindStorageBuffers(std::initializer_list<Buffer*> buffers) {
    WGPUBindGroupEntry* entries = new WGPUBindGroupEntry[buffers.size()];
    uint32_t i = 0;
    for (const Buffer* buffer : buffers) {
        assert(buffer != nullptr);
        assert(buffer->GetBufferUsage() == Buffer::BufferUsage::STORAGE_BUFFER || buffer->GetBufferUsage() == Buffer::BufferUsage::VERTEX_STORAGE_BUFFER);

        entries[i] = {};
        entries[i].binding = i;
        entries[i].buffer = static_cast<const WebGPUBuffer*>(buffer)->GetBuffer();
        entries[i].size = buffer->GetSize();

        ++i;
    }

    WGPUBindGroupDescriptor bindGroupDescriptor = {};
    bindGroupDescriptor.layout = GetCurrentShaderProgram()->GetBindGroupLayouts()[ShaderProgram::BindingType::STORAGE_BUFFER];
    bindGroupDescriptor.entryCount = buffers.size();
    bindGroupDescriptor.entries = entries;

    WGPUBindGroup storageBindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);

    if (inRenderPass) {
        wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, ShaderProgram::BindingType::STORAGE_BUFFER, storageBindGroup, 0, nullptr);
    } else {
        wgpuComputePassEncoderSetBindGroup(computePassEncoder, ShaderProgram::BindingType::STORAGE_BUFFER, storageBindGroup, 0, nullptr);
    }

    bindGroupsToRelease.push_back(storageBindGroup);
    delete[] entries;
}

void WebGPUCommandBuffer::BindMaterial(std::initializer_list<std::pair<Texture*, const Sampler*>> textures) {
    assert(inRenderPass);

    WGPUBindGroupEntry* entries = new WGPUBindGroupEntry[textures.size() * 2];
    unsigned int i = 0;
    for (auto& texture : textures) {
        assert(texture.first->GetType() != Texture::Type::RENDER_DEPTH);

        entries[i] = {};
        entries[i].binding = i;
        entries[i].textureView = static_cast<const WebGPUTexture*>(texture.first)->GetTextureView();

        entries[i + textures.size()] = {};
        entries[i + textures.size()].binding = i + textures.size();
        entries[i + textures.size()].sampler = static_cast<const WebGPUSampler*>(texture.second)->GetSampler();

        ++i;
    }

    WGPUBindGroupDescriptor bindGroupDescriptor = {};
    bindGroupDescriptor.layout = currentGraphicsPipeline->GetShaderProgram()->GetBindGroupLayouts()[ShaderProgram::BindingType::MATERIAL];
    bindGroupDescriptor.entryCount = textures.size() * 2;
    bindGroupDescriptor.entries = entries;

    WGPUBindGroup textureBindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);

    wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, ShaderProgram::BindingType::MATERIAL, textureBindGroup, 0, nullptr);

    bindGroupsToRelease.push_back(textureBindGroup);
    delete[] entries;
}

void WebGPUCommandBuffer::PushConstants(const void* data) {
    assert(data != nullptr);
    assert(GetCurrentShaderProgram()->GetPushConstantSize() > 0);

    // Acquire place in push constant buffer.
    if (currentPushConstantBuffer >= pushConstantBuffers.size()) {
        AllocatePushConstantBuffer();
    }

    if (pushConstantBuffers[currentPushConstantBuffer].currentPushConstant >= pushConstantsPerBuffer) {
        ++currentPushConstantBuffer;
    }

    if (currentPushConstantBuffer >= pushConstantBuffers.size()) {
        AllocatePushConstantBuffer();
    }

    // Copy the data.
    memcpy(&pushConstantBuffers[currentPushConstantBuffer].data[pushConstantSize * pushConstantBuffers[currentPushConstantBuffer].currentPushConstant], data, GetCurrentShaderProgram()->GetPushConstantSize());

    pushConstantBuffers[currentPushConstantBuffer].currentPushConstant++;

    uniformsHasChanged = true;
}

void WebGPUCommandBuffer::Draw(unsigned int vertexCount, unsigned int firstVertex) {
    assert(inRenderPass);

    UpdateUniforms();

    wgpuRenderPassEncoderDraw(renderPassEncoder, vertexCount, 1, firstVertex, 0);
}

void WebGPUCommandBuffer::DrawIndexed(unsigned int indexCount, unsigned int firstIndex, unsigned int baseVertex) {
    assert(inRenderPass);

    UpdateUniforms();

    wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, indexCount, 1, firstIndex, baseVertex, 0);
}

void WebGPUCommandBuffer::DrawIndexedInstanced(unsigned int indexCount, unsigned int instanceCount, unsigned int firstIndex, unsigned int baseVertex) {
    assert(inRenderPass);

    UpdateUniforms();

    wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, indexCount, instanceCount, firstIndex, baseVertex, 0);
}

void WebGPUCommandBuffer::BlitToSwapChain(Texture* texture) {
    assert(!inRenderPass);
    assert(texture != nullptr);

    EndComputePass();

    // Begin render pass.
    // BeginRenderPass requires a Texture to render to, but we're rendering to the swap chain, which doesn't have a Texture.
    // So manually begin the render pass here instead.
    WGPURenderPassColorAttachment colorAttachment = {};
    colorAttachment.view = renderer->GetCurrentFrame();
    colorAttachment.clearValue.r = 0.0;
    colorAttachment.clearValue.g = 0.0;
    colorAttachment.clearValue.b = 0.0;
    colorAttachment.clearValue.a = 0.0;
    colorAttachment.loadOp = WGPULoadOp_Clear;
    colorAttachment.storeOp = WGPUStoreOp_Store;

    WGPURenderPassDescriptor renderPassDescriptor = {};
    renderPassDescriptor.colorAttachmentCount = 1;
    renderPassDescriptor.colorAttachments = &colorAttachment;

    renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
    inRenderPass = true;

    renderPassDescription = {};
    renderPassDescription.hasColorAttachment = true;
    renderPassDescription.colorAttachmentFormat = renderer->GetSwapChainFormat();

    // Draw a full screen quad to blit the texture to the swap chain.
    BindGraphicsPipeline(blitGraphicsPipeline);
    SetViewportAndScissor(glm::uvec2(0, 0), texture->GetSize());
    BindMaterial({ {texture, blitSampler} });
    Draw(3, 0);

    // End render pass.
    EndRenderPass();
}

void WebGPUCommandBuffer::BindComputePipeline(ComputePipeline* computePipeline) {
    assert(!inRenderPass);
    assert(computePipeline != nullptr);

    if (!inComputePass) {
        // Begin compute pass.
        WGPUComputePassDescriptor computePassDescriptor = {};
        computePassEncoder = wgpuCommandEncoderBeginComputePass(commandEncoder, &computePassDescriptor);

        inComputePass = true;
    }

    // Get render pipeline.
    currentComputePipeline = static_cast<WebGPUComputePipeline*>(computePipeline);

    wgpuComputePassEncoderSetPipeline(computePassEncoder, currentComputePipeline->GetComputePipeline());

    // Bind empty bind groups for all empty bind group layouts.
    for (uint32_t binding = 0; binding < ShaderProgram::BindingType::BINDING_TYPES; ++binding) {
        if (currentComputePipeline->GetShaderProgram()->IsBindGroupLayoutEmpty(static_cast<ShaderProgram::BindingType>(binding))) {
            wgpuComputePassEncoderSetBindGroup(computePassEncoder, binding, emptyBindGroup, 0, nullptr);
        }
    }

    uniformsHasChanged = false;
}

void WebGPUCommandBuffer::Dispatch(const glm::uvec3& numGroups, const std::string& name) {
    assert(!inRenderPass);
    assert(inComputePass);
    assert(currentComputePipeline != nullptr);

    UpdateUniforms();

#ifndef NDEBUG
    wgpuComputePassEncoderInsertDebugMarker(computePassEncoder, name.c_str());
#endif
    wgpuComputePassEncoderDispatchWorkgroups(computePassEncoder, numGroups.x, numGroups.y, numGroups.z);
}

void WebGPUCommandBuffer::ClearBuffer(Buffer* buffer) {
    assert(!inRenderPass);
    assert(buffer != nullptr);

    EndComputePass();

    wgpuCommandEncoderClearBuffer(commandEncoder, static_cast<WebGPUBuffer*>(buffer)->GetBuffer(), 0, buffer->GetSize());
}

WGPUCommandEncoder WebGPUCommandBuffer::GetCommandEncoder() {
    return commandEncoder;
}

WGPUCommandBuffer WebGPUCommandBuffer::End() {
    assert(!inRenderPass);

    EndComputePass();

    // Upload push constant data.
    for (PushConstantBuffer& buffer : pushConstantBuffers) {
        if (buffer.currentPushConstant > 0) {
            wgpuQueueWriteBuffer(renderer->GetQueue(), buffer.buffer->GetBuffer(), 0, buffer.data, pushConstantSize * buffer.currentPushConstant);
        }
    }

    WGPUCommandBufferDescriptor commandBufferDescriptor = {};
    WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
    
    // Release bind groups.
    for (WGPUBindGroup bindGroup : bindGroupsToRelease) {
        wgpuBindGroupRelease(bindGroup);
    }
    bindGroupsToRelease.clear();

    return commandBuffer;
}

void WebGPUCommandBuffer::NextFrame() {
    // Allocate a new command encoder for each frame.
#if WEBGPU_BACKEND_DAWN
    if (commandEncoder != nullptr) {
        wgpuCommandEncoderRelease(commandEncoder);
    }
#endif

    WGPUCommandEncoderDescriptor encoderDescriptor = {};
    commandEncoder = wgpuDeviceCreateCommandEncoder(device, &encoderDescriptor);

    // Clear push constant data.
    for (PushConstantBuffer& buffer : pushConstantBuffers) {
        buffer.currentPushConstant = 0;
    }
    currentPushConstantBuffer = 0;
}

WGPURenderPassEncoder WebGPUCommandBuffer::GetRenderPassEncoder() {
    assert(inRenderPass);

    return renderPassEncoder;
}

void WebGPUCommandBuffer::AllocatePushConstantBuffer() {
    PushConstantBuffer pushConstantBuffer = {};
    pushConstantBuffer.buffer = static_cast<WebGPUBuffer*>(renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, pushConstantSize * pushConstantsPerBuffer));

    pushConstantBuffers.push_back(pushConstantBuffer);
}

void WebGPUCommandBuffer::UpdateUniforms() {
    if (!uniformsHasChanged) {
        return;
    }

    const WebGPUShaderProgram* shaderProgram;

    if (inRenderPass) {
        assert(currentGraphicsPipeline != nullptr);

        shaderProgram = currentGraphicsPipeline->GetShaderProgram();
    } else {
        assert(inComputePass);
        assert(currentComputePipeline != nullptr);

        shaderProgram = currentComputePipeline->GetShaderProgram();
    }

    WGPUBindGroupDescriptor bindGroupDescriptor = {};
    WGPUBindGroupEntry entries[2];
    if (shaderProgram->HasUniformsBuffer()) {
        entries[bindGroupDescriptor.entryCount] = {};
        entries[bindGroupDescriptor.entryCount].binding = 0;
        entries[bindGroupDescriptor.entryCount].buffer = currentUniformBuffer->GetBuffer();
        entries[bindGroupDescriptor.entryCount].size = currentUniformBuffer->GetSize();

        ++bindGroupDescriptor.entryCount;
    }

    if (shaderProgram->GetPushConstantSize() > 0) {
        // Find the push constant buffer and the offset into it.
        uint32_t whichBuffer = currentPushConstantBuffer;
        uint32_t offset = pushConstantBuffers[whichBuffer].currentPushConstant;
        if (offset == 0) {
            assert(whichBuffer > 0);
            --whichBuffer;
            offset = pushConstantsPerBuffer;
        }
        --offset;
        offset *= pushConstantSize;

        entries[bindGroupDescriptor.entryCount] = {};
        entries[bindGroupDescriptor.entryCount].binding = 1;
        entries[bindGroupDescriptor.entryCount].buffer = pushConstantBuffers[whichBuffer].buffer->GetBuffer();
        entries[bindGroupDescriptor.entryCount].offset = offset;
        entries[bindGroupDescriptor.entryCount].size = pushConstantSize;

        ++bindGroupDescriptor.entryCount;
    }

    if (bindGroupDescriptor.entryCount > 0) {
        bindGroupDescriptor.layout = shaderProgram->GetBindGroupLayouts()[ShaderProgram::BindingType::UNIFORMS];
        bindGroupDescriptor.entries = entries;
        WGPUBindGroup uniformBindGroup = wgpuDeviceCreateBindGroup(device, &bindGroupDescriptor);

        if (inRenderPass) {
            wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, ShaderProgram::BindingType::UNIFORMS, uniformBindGroup, 0, nullptr);
        } else {
            wgpuComputePassEncoderSetBindGroup(computePassEncoder, ShaderProgram::BindingType::UNIFORMS, uniformBindGroup, 0, nullptr);
        }
        bindGroupsToRelease.push_back(uniformBindGroup);
    }

    uniformsHasChanged = false;
}

void WebGPUCommandBuffer::EndComputePass() {
    if (!inComputePass) {
        return;
    }

    wgpuComputePassEncoderEnd(computePassEncoder);
#if WEBGPU_BACKEND_DAWN
    wgpuComputePassEncoderRelease(computePassEncoder);
#endif

    inComputePass = false;
}

const WebGPUShaderProgram* WebGPUCommandBuffer::GetCurrentShaderProgram() const {
    if (inRenderPass) {
        assert(currentGraphicsPipeline != nullptr);

        return currentGraphicsPipeline->GetShaderProgram();
    } else {
        assert(inComputePass);
        assert(currentComputePipeline != nullptr);

        return currentComputePipeline->GetShaderProgram();
    }
}

} // namespace Video
