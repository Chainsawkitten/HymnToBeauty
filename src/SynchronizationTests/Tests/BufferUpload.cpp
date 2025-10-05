#include "BufferUpload.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/LowLevelRenderer/Interface/Buffer.hpp>
#include <random>
#include <Utility/Log.hpp>

using namespace Video;

const char* BufferUpload::GetName() const {
    return "Buffer upload";
}

void BufferUpload::Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) {
    this->renderer = renderer;
    this->screenSize = screenSize;

    mainRenderTarget = renderer->CreateRenderTarget(screenSize, Texture::Format::R11G11B10);
    commandBuffer = renderer->CreateCommandBuffer();

    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned int> char_distribution(0, 255);

    std::size_t totalSize = 0;

    for (uint32_t i = 0; i < bufferCount; ++i) {
        bufferInfos[i].size = ((i / buffersPerSize) + 1) * bufferSizeIncrement;
        totalSize += bufferInfos[i].size;
        bufferInfos[i].data = new unsigned char[bufferInfos[i].size];
        for (uint32_t j = 0; j < bufferInfos[i].size; ++j) {
            bufferInfos[i].data[j] = static_cast<unsigned char>(char_distribution(generator));
        }
    }

    Log(Log::INFO) << "Total size: " << totalSize << "\n";
}

void BufferUpload::Frame() {
    // Perform buffer uploads.
    for (uint32_t i = 0; i < bufferCount; ++i) {
        renderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, bufferInfos[i].size, bufferInfos[i].data);
    }

    // First render pass simply clears.
    commandBuffer->BeginRenderPass(mainRenderTarget, RenderPass::LoadOperation::CLEAR);
    commandBuffer->EndRenderPass();

    commandBuffer->BlitToSwapChain(mainRenderTarget);

    renderer->Submit(commandBuffer);
}

void BufferUpload::Shutdown() {
    for (uint32_t i = 0; i < bufferCount; ++i) {
        delete[] bufferInfos[i].data;
    }
    
    delete mainRenderTarget;
    delete commandBuffer;
}
