#pragma once

#include "../Framework/SynchronizationTest.hpp"

namespace Video {
    class Texture;
    class CommandBuffer;
    class Shader;
    class ShaderProgram;
    class GraphicsPipeline;
    class VertexDescription;
    class Buffer;
}

namespace Meshes {
    class Cube;
}

class BufferUpload : public SynchronizationTest {
  public:
    const char* GetName() const override;
    void Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) override;
    void Frame() override;
    void Shutdown() override;

  private:
      struct BufferInfo {
          uint32_t size;
          unsigned char* data;
      };

      static const uint32_t bufferCount = 1000;
      static const uint32_t buffersPerSize = 10;
      static const uint32_t bufferSizeIncrement = 1024;
      BufferInfo bufferInfos[bufferCount];

      Video::LowLevelRenderer* renderer;
      Video::Texture* mainRenderTarget;
      Video::CommandBuffer* commandBuffer;

      glm::uvec2 screenSize;
};
