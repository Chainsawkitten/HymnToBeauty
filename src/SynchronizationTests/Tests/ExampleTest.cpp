#include "ExampleTest.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>

using namespace Video;

const char* ExampleTest::GetName() const {
    return "Example test";
}

void ExampleTest::Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) {
    this->renderer = renderer;

    mainRenderTarget = renderer->CreateRenderTarget(screenSize, Texture::Format::R11G11B10);
    commandBuffer = renderer->CreateCommandBuffer();
}

void ExampleTest::Frame() {
    commandBuffer->BeginRenderPass(mainRenderTarget);
    commandBuffer->EndRenderPass();
    commandBuffer->BlitToSwapChain(mainRenderTarget);

    renderer->Submit(commandBuffer);
}

void ExampleTest::Shutdown() {
    delete mainRenderTarget;
    delete commandBuffer;
}

