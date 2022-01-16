#include "OpenGLRenderPassAllocator.hpp"

#include "OpenGLRenderPass.hpp"

namespace Video {

OpenGLRenderPassAllocator::OpenGLRenderPassAllocator() {

}

OpenGLRenderPassAllocator::~OpenGLRenderPassAllocator() {

}

RenderPass* OpenGLRenderPassAllocator::AllocateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) {
    return new OpenGLRenderPass(colorAttachment, colorLoadOperation, depthAttachment, depthLoadOperation);
}

RenderPass* OpenGLRenderPassAllocator::AllocateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples) {
    return new OpenGLRenderPass(size, msaaSamples);
}

}
