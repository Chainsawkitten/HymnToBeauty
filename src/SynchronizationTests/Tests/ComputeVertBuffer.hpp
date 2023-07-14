#pragma once

#include "../Framework/SynchronizationTest.hpp"

namespace Video {
    class Texture;
    class CommandBuffer;
    class Shader;
    class ShaderProgram;
    class GraphicsPipeline;
    class ComputePipeline;
    class VertexDescription;
    class Buffer;
    class GeometryBinding;
}

class ComputeVertBuffer : public SynchronizationTest {
  public:
    const char* GetName() const override;
    void Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) override;
    void Frame() override;
    void Shutdown() override;

  private:
      Video::LowLevelRenderer* renderer;
      Video::Texture* mainRenderTarget;
      Video::Texture* depthBuffer;
      Video::Texture* offsetImage;
      Video::CommandBuffer* commandBuffer;

      glm::uvec2 screenSize;

      Video::VertexDescription* vertexDescription;
      Video::Shader* planeVertexShader;
      Video::Shader* planeFragmentShader;
      Video::ShaderProgram* planeShaderProgram;
      Video::GraphicsPipeline* planePipeline;

      Video::Shader* offsetShader;
      Video::ShaderProgram* offsetShaderProgram;
      Video::ComputePipeline* offsetPipeline;

      uint32_t planeIndexCount;
      Video::Buffer* planeVertexBuffer;
      Video::Buffer* planeIndexBuffer;
      Video::GeometryBinding* planeGeometryBinding;
      Video::Texture* planeAlbedo;

      Video::Buffer* offsetInstanceBuffer;
      Video::Buffer* offsetBuffer;
      Video::Buffer* offsetUniformBuffer;

      Video::Buffer* matrixBuffer;
      Video::Buffer* instanceBuffer;
      Video::Buffer* uniformBuffer;
      Video::Buffer* lightBuffer;

      const uint32_t instanceCount = 100;
};
