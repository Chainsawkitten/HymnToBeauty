#include "WebGPUTexture.hpp"

#include "WebGPURenderer.hpp"
#include "WebGPUCommandBuffer.hpp"
#include "WebGPUSampler.hpp"
#include "WebGPUGraphicsPipeline.hpp"
#include "WebGPUShaderProgram.hpp"

#include <cassert>
#include <Utility/Log.hpp>

namespace Video {

WebGPUTexture::WebGPUTexture(WebGPURenderer& renderer, const glm::uvec2 size, Texture::Type type, Texture::Format format, int components, unsigned char* data, uint32_t sampleCount) : Texture(type, size, format) {
    assert(size.x > 0 && size.y > 0);
    assert((data != nullptr && components >= 1 && components <= 4) || (data == nullptr && components == 0));
    assert((type == Texture::Type::COLOR && data != nullptr) || (type != Texture::Type::COLOR && data == nullptr));
    assert(type == Texture::Type::RENDER_COLOR || sampleCount == 1);

    this->sampleCount = sampleCount;

    // Create texture.
    WGPUTextureDescriptor textureDescriptor = {};
    textureDescriptor.size.width = size.x;
    textureDescriptor.size.height = size.y;
    textureDescriptor.size.depthOrArrayLayers = 1;
    textureDescriptor.dimension = WGPUTextureDimension_2D;
    textureDescriptor.sampleCount = sampleCount;

    switch (type) {
    case Texture::Type::COLOR:
        textureDescriptor.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
        break;
    case Texture::Type::RENDER_COLOR:
        textureDescriptor.usage = WGPUTextureUsage_RenderAttachment | WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopySrc;
        break;
    case Texture::Type::RENDER_DEPTH:
        textureDescriptor.usage = WGPUTextureUsage_RenderAttachment;
        break;
    default:
        assert(false);
    }

    switch (format) {
    case Texture::Format::R8:
        assert(type != Texture::Type::RENDER_DEPTH);
        textureFormat = WGPUTextureFormat_R8Unorm;
        break;
    case Texture::Format::R8G8B8A8:
        assert(type != Texture::Type::RENDER_DEPTH);
        textureFormat = WGPUTextureFormat_RGBA8Unorm;
        break;
    case Texture::Format::R11G11B10:
        assert(type != Texture::Type::RENDER_DEPTH);
        textureFormat = renderer.HasR11G11B10() ? WGPUTextureFormat_RG11B10Ufloat : WGPUTextureFormat_RGBA16Float;
        break;
    case Texture::Format::R16G16B16A16:
        assert(type != Texture::Type::RENDER_DEPTH);
        textureFormat = WGPUTextureFormat_RGBA16Float;
        break;
    case Texture::Format::D32:
        assert(type == Texture::Type::RENDER_DEPTH);
        textureFormat = WGPUTextureFormat_Depth32Float;
        break;
    default:
        assert(false);
    }
    textureDescriptor.format = textureFormat;

    // Calculate the number of mip levels.
    textureDescriptor.mipLevelCount = (type == Texture::Type::COLOR ? static_cast<uint32_t>(std::log2(std::max(size.x, size.y))) + 1 : 1);

    texture = wgpuDeviceCreateTexture(renderer.GetDevice(), &textureDescriptor);

    // Create texture view.
    WGPUTextureViewDescriptor textureViewDescriptor = {};
    textureViewDescriptor.format = textureDescriptor.format;
    textureViewDescriptor.dimension = WGPUTextureViewDimension_2D;
    textureViewDescriptor.baseMipLevel = 0;
    textureViewDescriptor.mipLevelCount = textureDescriptor.mipLevelCount;
    textureViewDescriptor.baseArrayLayer = 0;
    textureViewDescriptor.arrayLayerCount = 1;
    textureViewDescriptor.aspect = WGPUTextureAspect_All;
    textureView = wgpuTextureCreateView(texture, &textureViewDescriptor);

    if (type == Texture::Type::COLOR) {
        // Upload data.
        WGPUImageCopyTexture copyTexture = {};
        copyTexture.texture = texture;
        copyTexture.mipLevel = 0;
        copyTexture.aspect = WGPUTextureAspect_All;

        const size_t dataSize = size.x * size.y * 4;
        unsigned char* stagingData = new unsigned char[dataSize];
        for (uint32_t i = 0; i < size.x * size.y; ++i) {
            stagingData[i * 4] = data[i * components];
            stagingData[i * 4 + 1] = (components > 1 ? data[i * components + 1] : 0);
            stagingData[i * 4 + 2] = (components > 2 ? data[i * components + 2] : 0);
            stagingData[i * 4 + 3] = (components > 3 ? data[i * components + 3] : 255);
        }

        WGPUTextureDataLayout dataLayout = {};
        dataLayout.offset = 0;
        dataLayout.bytesPerRow = size.x * 4;
        dataLayout.rowsPerImage = size.y;

        WGPUExtent3D writeSize = {};
        writeSize.width = size.x;
        writeSize.height = size.y;
        writeSize.depthOrArrayLayers = 1;

        wgpuQueueWriteTexture(renderer.GetQueue(), &copyTexture, stagingData, dataSize, &dataLayout, &writeSize);

        delete[] stagingData;
        
        GenerateMipMaps(renderer, size, textureDescriptor.mipLevelCount);
    }
}

WebGPUTexture::~WebGPUTexture() {
    wgpuTextureViewRelease(textureView);
    wgpuTextureRelease(texture);
}

WGPUTexture WebGPUTexture::GetTexture() const {
    return texture;
}

WGPUTextureView WebGPUTexture::GetTextureView() const {
    return textureView;
}

WGPUTextureFormat WebGPUTexture::GetTextureFormat() const {
    return textureFormat;
}

uint32_t WebGPUTexture::GetSampleCount() const {
    return sampleCount;
}

void WebGPUTexture::GenerateMipMaps(WebGPURenderer& renderer, const glm::uvec2& size, uint32_t mipLevels) {
    GraphicsPipeline* blitPipeline = renderer.GetBlitGraphicsPipeline();
    WebGPUCommandBuffer* commandBuffer = static_cast<WebGPUCommandBuffer*>(renderer.CreateCommandBuffer());
    const Sampler* sampler = renderer.GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE);

    // Calculate the size of the second largest mip level.
    uint32_t mipWidth = size.x;
    uint32_t mipHeight = size.y;

    if (mipWidth > 1)
        mipWidth /= 2;
    if (mipHeight > 1)
        mipHeight /= 2;

    // Get render target to render mips to. We'll reuse a single render target, rather than allocating a new one per mip, so it has to be the size of the largest mip we'll render.
    Texture* renderTarget = renderer.CreateRenderTarget(glm::uvec2(mipWidth, mipHeight), GetFormat());
    WGPUBindGroup* textureBindGroups = new WGPUBindGroup[mipLevels];
    WGPUTextureView* mipTextureViews = new WGPUTextureView[mipLevels];

    // Generate mipmaps.
    for (uint32_t i = 1; i < mipLevels; ++i) {
        // Render mip level.
        commandBuffer->BeginRenderPass(renderTarget, RenderPass::LoadOperation::DONT_CARE, nullptr, RenderPass::LoadOperation::DONT_CARE, "Mip generation");
        commandBuffer->BindGraphicsPipeline(blitPipeline);
        commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), glm::uvec2(mipWidth, mipHeight));
        
        // Bind the texture. We can't use commandBuffer->BindMaterial as we want to bind a specific mip level.
        {
            // Create a texture view for this mip level.
            WGPUTextureViewDescriptor textureViewDescriptor = {};
            textureViewDescriptor.format = textureFormat;
            textureViewDescriptor.dimension = WGPUTextureViewDimension_2D;
            textureViewDescriptor.baseMipLevel = i - 1;
            textureViewDescriptor.mipLevelCount = 1;
            textureViewDescriptor.baseArrayLayer = 0;
            textureViewDescriptor.arrayLayerCount = 1;
            textureViewDescriptor.aspect = WGPUTextureAspect_All;
            mipTextureViews[i] = wgpuTextureCreateView(texture, &textureViewDescriptor);

            // Bind this texture view, and the sampler.
            WGPUBindGroupEntry entries[2];

            entries[0] = {};
            entries[0].binding = 0;
            entries[0].textureView = mipTextureViews[i];

            entries[1] = {};
            entries[1].binding = 1;
            entries[1].sampler = static_cast<const WebGPUSampler*>(sampler)->GetSampler();

            WGPUBindGroupDescriptor bindGroupDescriptor = {};
            bindGroupDescriptor.layout = static_cast<WebGPUGraphicsPipeline*>(blitPipeline)->GetShaderProgram()->GetBindGroupLayouts()[ShaderProgram::BindingType::MATERIAL];
            bindGroupDescriptor.entryCount = 2;
            bindGroupDescriptor.entries = entries;

            textureBindGroups[i] = wgpuDeviceCreateBindGroup(renderer.GetDevice(), &bindGroupDescriptor);

            wgpuRenderPassEncoderSetBindGroup(commandBuffer->GetRenderPassEncoder(), ShaderProgram::BindingType::MATERIAL, textureBindGroups[i], 0, nullptr);
        }

        commandBuffer->Draw(3, 0);

        commandBuffer->EndRenderPass();

        // Copy to mip.
        WGPUImageCopyTexture source = {};
        source.texture = static_cast<WebGPUTexture*>(renderTarget)->GetTexture();
        source.mipLevel = 0;
        source.aspect = WGPUTextureAspect_All;

        WGPUImageCopyTexture destination = {};
        destination.texture = texture;
        destination.mipLevel = i;
        destination.aspect = WGPUTextureAspect_All;

        WGPUExtent3D copySize = {};
        copySize.width = mipWidth;
        copySize.height = mipHeight;
        copySize.depthOrArrayLayers = 1;

        wgpuCommandEncoderCopyTextureToTexture(commandBuffer->GetCommandEncoder(), &source, &destination, &copySize);

        // Calculate the size of the next mip level.
        if (mipWidth > 1)
            mipWidth /= 2;
        if (mipHeight > 1)
            mipHeight /= 2;
    }

    renderer.Submit(commandBuffer);

    // Cleanup
    for (uint32_t i = 1; i < mipLevels; ++i) {
        wgpuBindGroupRelease(textureBindGroups[i]);
        wgpuTextureViewRelease(mipTextureViews[i]);
    }
    delete[] textureBindGroups;
    delete[] mipTextureViews;

    renderer.FreeRenderTarget(renderTarget);

    delete commandBuffer;
}

}
