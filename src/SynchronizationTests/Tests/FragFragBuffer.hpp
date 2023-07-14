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
    class Isocahedron;
}

namespace Meshes {
    class Cube;
}

class FragFragBuffer : public SynchronizationTest {
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

      Video::Shader* lightTilingVertexShader;
      Video::Shader* lightTilingFragmentShader;
      Video::ShaderProgram* lightTilingShaderProgram;
      Video::GraphicsPipeline* lightTilingPipeline;

      Meshes::Cube* cube;
      Video::Texture* cubeAlbedo;

      Video::Buffer* matrixBuffer;
      Video::Buffer* instanceBuffer;
      Video::Buffer* uniformBuffer;
      Video::Buffer* lightBuffer;
      Video::Buffer* tileMatrixBuffer;
      Video::Buffer* tileBuffer;
      Video::Buffer* tilingUniformBuffer;

      Video::Isocahedron* isocahedron;

      const uint32_t instanceCount = 6000;
      const uint32_t tileSize = 16;
      glm::uvec2 tiles;
};
