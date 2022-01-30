#include "RenderTargetAllocator.hpp"

#include "Texture.hpp"
#include <Utility/Log.hpp>

namespace Video {

RenderTargetAllocator::RenderTargetAllocator(uint8_t frames) {
    this->frames = frames;
}

RenderTargetAllocator::~RenderTargetAllocator() {
    for (auto& it : freeRenderTargets) {
        while (!it.second.empty()) {
            delete it.second.front().texture;
            it.second.pop_front();
        }
    }
}

void RenderTargetAllocator::BeginFrame() {
    // Delete render targets that haven't been used for x frames.
    for (auto& it : freeRenderTargets) {
        auto i = it.second.begin();
        while (i != it.second.end()) {
            if (i->age++ > frames) {
                delete i->texture;
                i = it.second.erase(i);
            } else {
                ++i;
            }
        }
    }
}

Texture* RenderTargetAllocator::CreateRenderTarget(const glm::uvec2& size, Texture::Format format) {
    RenderTargetInfo renderTargetInfo;
    renderTargetInfo.size = size;
    renderTargetInfo.format = format;

    std::deque<RenderTarget>& renderTargets = freeRenderTargets[renderTargetInfo];

    if (!renderTargets.empty()) {
        // Use existing render target.
        Texture* texture = renderTargets.front().texture;
        renderTargets.pop_front();
        return texture;
    } else {
        // Allocate new render target.
        return AllocateRenderTarget(size, format);
    }
}

void RenderTargetAllocator::FreeRenderTarget(Texture* renderTarget) {
    RenderTargetInfo renderTargetInfo;
    renderTargetInfo.size = renderTarget->GetSize();
    renderTargetInfo.format = renderTarget->GetFormat();

    RenderTarget rt = {renderTarget, 0};

    freeRenderTargets[renderTargetInfo].push_back(rt);
}

}
