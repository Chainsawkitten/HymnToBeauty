#include "OpenGLRenderTargetAllocator.hpp"

#include "OpenGLTexture.hpp"
#include <cassert>

namespace Video {

OpenGLRenderTargetAllocator::OpenGLRenderTargetAllocator(uint8_t frames) : RenderTargetAllocator(frames) {}

OpenGLRenderTargetAllocator::~OpenGLRenderTargetAllocator() {}

Texture* OpenGLRenderTargetAllocator::AllocateRenderTarget(const glm::uvec2& size, Texture::Format format) {
    Texture::Type type;
    switch (format) {
    case Texture::Format::R11G11B10:
    case Texture::Format::R8G8B8A8:
        type = Texture::Type::RENDER_COLOR;
        break;
    case Texture::Format::D32:
        type = Texture::Type::RENDER_DEPTH;
        break;
    default:
        assert(false);
    }

    return new OpenGLTexture(size, type, format);
}

} // namespace Video
