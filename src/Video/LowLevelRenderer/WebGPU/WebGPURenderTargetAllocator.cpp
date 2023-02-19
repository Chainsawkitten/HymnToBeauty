#include "WebGPURenderTargetAllocator.hpp"

#include "WebGPUTexture.hpp"

namespace Video {

static const uint8_t framesBeforeFree = 2;

WebGPURenderTargetAllocator::WebGPURenderTargetAllocator(WebGPURenderer& renderer) : RenderTargetAllocator(framesBeforeFree, nullptr), renderer(renderer) {
    
}

WebGPURenderTargetAllocator::~WebGPURenderTargetAllocator() {

}

void WebGPURenderTargetAllocator::BeginFrame() {
    RenderTargetAllocator::BeginFrame();

    // Delete attachmentless render targets that haven't been used for x frames.
    for (auto& it : freeAttachmentlessRenderTargets) {
        auto i = it.second.begin();
        while (i != it.second.end()) {
            if (i->age++ > framesBeforeFree) {
                delete i->texture;
                i = it.second.erase(i);
            } else {
                ++i;
            }
        }
    }
}

WebGPUTexture* WebGPURenderTargetAllocator::CreateAttachmentlessRenderTarget(const glm::uvec2& size, uint32_t sampleCount) {
    assert(size.x > 0 && size.y > 0);

    AttachmentlessRenderTargetInfo renderTargetInfo;
    renderTargetInfo.size = size;
    renderTargetInfo.sampleCount = sampleCount;

    std::deque<AttachmentlessRenderTarget>& renderTargets = freeAttachmentlessRenderTargets[renderTargetInfo];

    if (!renderTargets.empty()) {
        // Use existing render target.
        WebGPUTexture* texture = renderTargets.front().texture;
        renderTargets.pop_front();
        return texture;
    } else {
        // Allocate new render target.
        return new WebGPUTexture(renderer, size, Texture::Type::RENDER_COLOR, Texture::Format::R8, 0, nullptr, sampleCount);
    }
}

void WebGPURenderTargetAllocator::FreeAttachmentlessRenderTarget(WebGPUTexture* renderTarget) {
    assert(renderTarget != nullptr);

    AttachmentlessRenderTargetInfo renderTargetInfo;
    renderTargetInfo.size = renderTarget->GetSize();
    renderTargetInfo.sampleCount = renderTarget->GetSampleCount();

    AttachmentlessRenderTarget rt = {renderTarget, 0};

    freeAttachmentlessRenderTargets[renderTargetInfo].push_back(rt);
}

Texture* WebGPURenderTargetAllocator::AllocateRenderTarget(const glm::uvec2& size, Texture::Format format) {
    Texture::Type type;
    switch (format) {
    case Texture::Format::R8:
    case Texture::Format::R11G11B10:
    case Texture::Format::R8G8B8A8:
    case Texture::Format::R16G16B16A16:
        type = Texture::Type::RENDER_COLOR;
        break;
    case Texture::Format::D32:
        type = Texture::Type::RENDER_DEPTH;
        break;
    default:
        assert(false);
    }

    return new WebGPUTexture(renderer, size, type, format);
}

void WebGPURenderTargetAllocator::FreeRenderPasses(Texture* renderTarget) {
    // Do nothing, as we don't have render passes.
}

}
