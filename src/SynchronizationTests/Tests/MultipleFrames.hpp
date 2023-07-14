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

class MultipleFrames : public SynchronizationTest {
  public:
    const char* GetName() const override;
    void Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) override;
    void Frame() override;
    void Shutdown() override;

  private:
      Video::LowLevelRenderer* renderer;
      Video::Texture* mainRenderTarget;
      Video::Texture* depthBuffer;
      Video::CommandBuffer* commandBuffer;

      glm::uvec2 screenSize;

      Video::VertexDescription* vertexDescription;
      Video::Shader* cubeVertexShader;
      Video::Shader* cubeFragmentShader;
      Video::ShaderProgram* cubeShaderProgram;
      Video::GraphicsPipeline* cubePipeline;

      Meshes::Cube* cube;
      Video::Texture* cubeAlbedo;

      Video::Buffer* instanceBuffer;
      Video::Buffer* uniformBuffer;
      Video::Buffer* lightBuffer;

      const uint32_t instanceCount = 5000;
};
