#include "WebGPUGraphicsPipeline.hpp"

#include "WebGPURenderer.hpp"
#include "WebGPUShader.hpp"
#include "WebGPUShaderProgram.hpp"
#include "WebGPUVertexDescription.hpp"
#include "WebGPUTexture.hpp"

#include <cassert>
#include <Utility/Log.hpp>

namespace Video {

WebGPUGraphicsPipeline::WebGPUGraphicsPipeline(WebGPURenderer& renderer, const ShaderProgram* shaderProgram, const Configuration& configuration, const VertexDescription* vertexDescription) {
    assert(shaderProgram != nullptr);
    assert(configuration.polygonMode != PolygonMode::LINE);
    assert(!configuration.conservativeRasterization);

    this->device = renderer.GetDevice();

    webGPUShaderProgram = static_cast<const WebGPUShaderProgram*>(shaderProgram);

    pipelineDescriptor = {};
    
    // Pipeline layout.
    WGPUPipelineLayoutDescriptor pipelineLayoutDescriptor = {};
    pipelineLayoutDescriptor.bindGroupLayoutCount = ShaderProgram::BindingType::BINDING_TYPES;
    pipelineLayoutDescriptor.bindGroupLayouts = webGPUShaderProgram->GetBindGroupLayouts();
    pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &pipelineLayoutDescriptor);
    pipelineDescriptor.layout = pipelineLayout;

    // Vertex state.
    if (vertexDescription != nullptr) {
        vertexBufferLayout = static_cast<const WebGPUVertexDescription*>(vertexDescription)->GetVertexBufferLayout();
        pipelineDescriptor.vertex.bufferCount = 1;
        pipelineDescriptor.vertex.buffers = &vertexBufferLayout;
    }

    // Primitive state.
    if (configuration.depthClamp && renderer.HasDepthClipControl()) {
        depthClipControl = {};
        depthClipControl.chain.sType = WGPUSType_PrimitiveDepthClipControl;
        depthClipControl.unclippedDepth = true;

        pipelineDescriptor.primitive.nextInChain = reinterpret_cast<const WGPUChainedStruct*>(&depthClipControl);
    }

    pipelineDescriptor.primitive.frontFace = WGPUFrontFace_CCW;

    switch (configuration.primitiveType) {
    case PrimitiveType::POINT:
        pipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_PointList;
        break;
    case PrimitiveType::LINE:
        pipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_LineList;
        break;
    case PrimitiveType::TRIANGLE:
        pipelineDescriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        break;
    }

    switch (configuration.cullFace) {
    case CullFace::NONE:
        pipelineDescriptor.primitive.cullMode = WGPUCullMode_None;
        break;
    case CullFace::BACK:
        pipelineDescriptor.primitive.cullMode = WGPUCullMode_Back;
        break;
    case CullFace::FRONT:
        pipelineDescriptor.primitive.cullMode = WGPUCullMode_Front;
        break;
    }

    // Depth-stencil state.
    depthStencilState = {};
    depthStencilState.stencilFront.compare = WGPUCompareFunction_Always;
    depthStencilState.stencilBack.compare = WGPUCompareFunction_Always;

    switch (configuration.depthComparison) {
    case DepthComparison::LESS:
        depthStencilState.depthCompare = WGPUCompareFunction_Less;
        break;
    case DepthComparison::LESS_EQUAL:
        depthStencilState.depthCompare = WGPUCompareFunction_LessEqual;
        break;
    case DepthComparison::ALWAYS:
        depthStencilState.depthCompare = WGPUCompareFunction_Always;
        break;
    }

    switch (configuration.depthMode) {
    case DepthMode::NONE:
        depthStencilState.depthWriteEnabled = false;
        depthStencilState.depthCompare = WGPUCompareFunction_Always;
        break;
    case DepthMode::TEST:
        depthStencilState.depthWriteEnabled = false;
        break;
    case DepthMode::TEST_WRITE:
        depthStencilState.depthWriteEnabled = true;
        break;
    }

    // Multisample state.
    pipelineDescriptor.multisample.count = 1;
    pipelineDescriptor.multisample.mask = ~0u;

    // Fragment state.
    fragmentState = {};
    colorTargetState = {};
    colorTargetState.writeMask = WGPUColorWriteMask_All;

    blendState = {};
    switch (configuration.blendMode) {
    case BlendMode::NONE:
        colorTargetState.blend = nullptr;
        break;
    case BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA:
        blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;
        blendState.alpha.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.alpha.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.alpha.operation = WGPUBlendOperation_Add;
        colorTargetState.blend = &blendState;
        break;
    case BlendMode::ONE_ONE_MINUS_SRC_ALPHA:
        blendState.color.srcFactor = WGPUBlendFactor_One;
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;
        blendState.alpha.srcFactor = WGPUBlendFactor_One;
        blendState.alpha.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.alpha.operation = WGPUBlendOperation_Add;
        colorTargetState.blend = &blendState;
        break;
    }

    pipelineDescriptor.fragment = &fragmentState;

    // Shader stages.
    const std::vector<const WebGPUShader*>& shaders = webGPUShaderProgram->GetShaders();
    for (const WebGPUShader* shader : shaders) {
        switch (shader->GetShaderStage()) {
        case WGPUShaderStage_Vertex:
            pipelineDescriptor.vertex.module = shader->GetShaderModule();
            pipelineDescriptor.vertex.entryPoint = "main";
            break;
        case WGPUShaderStage_Fragment:
            fragmentState.module = shader->GetShaderModule();
            fragmentState.entryPoint = "main";
            break;
        default:
            assert(false);
        }
    }
}

WebGPUGraphicsPipeline::~WebGPUGraphicsPipeline() {
    for (auto it : pipelines) {
        wgpuRenderPipelineRelease(it.second);
    }
    wgpuPipelineLayoutRelease(pipelineLayout);
}

WGPURenderPipeline WebGPUGraphicsPipeline::GetRenderPipeline(const RenderPassDescription& renderPassDescription) {
    auto it = pipelines.find(renderPassDescription);
    if (it != pipelines.end()) {
        // Use compatible pipeline.
        return it->second;
    } else {
        // Set state that is specific to this render pass.
        if (renderPassDescription.hasColorAttachment) {
            colorTargetState.format = renderPassDescription.colorAttachmentFormat;

            fragmentState.targetCount = 1;
            fragmentState.targets = &colorTargetState;
        } else {
            fragmentState.targetCount = 0;
        }

        if (renderPassDescription.hasDepthAttachment) {
            depthStencilState.format = renderPassDescription.depthAttachmentFormat;
            pipelineDescriptor.depthStencil = &depthStencilState;
        } else {
            pipelineDescriptor.depthStencil = nullptr;
        }

        // Emulation of attachmentless.
        if (!renderPassDescription.hasColorAttachment && !renderPassDescription.hasDepthAttachment) {
            colorTargetState.format = WGPUTextureFormat_R8Unorm;
            colorTargetState.writeMask = WGPUColorWriteMask_None;

            fragmentState.targetCount = 1;
            fragmentState.targets = &colorTargetState;

            pipelineDescriptor.multisample.count = renderPassDescription.attachmentlessMsaa;
        } else {
            pipelineDescriptor.multisample.count = 1;
        }

        // Create pipeline.
        WGPURenderPipeline pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDescriptor);
        pipelines[renderPassDescription] = pipeline;

        // Reset write mask.
        colorTargetState.writeMask = WGPUColorWriteMask_All;

        return pipeline;
    }
}

const WebGPUShaderProgram* WebGPUGraphicsPipeline::GetShaderProgram() const {
    return webGPUShaderProgram;
}

}
