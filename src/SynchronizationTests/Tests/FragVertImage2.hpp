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
    class GeometryBinding;
}

namespace Meshes {
    class Cube;
}

class FragVertImage2 : public SynchronizationTest {
  public:
    const char* GetName() const override;
    void Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) override;
    void Frame() override;
    void Shutdown() override;

  private:
      Video::LowLevelRenderer* renderer;
      Video::Texture* mainRenderTarget;
      Video::Texture* unrelatedRenderTarget;
      Video::Texture* depthBuffer;
      Video::Texture* offsetBuffer;
      Video::CommandBuffer* commandBuffer;

      glm::uvec2 screenSize;

      Video::VertexDescription* vertexDescription;
      Video::Shader* planeVertexShader;
      Video::Shader* planeFragmentShader;
      Video::ShaderProgram* planeShaderProgram;
      Video::GraphicsPipeline* planePipeline;

      Video::Shader* offsetVertexShader;
      Video::Shader* offsetFragmentShader;
      Video::ShaderProgram* offsetShaderProgram;
      Video::GraphicsPipeline* offsetPipeline;

      uint32_t planeIndexCount;
      Video::Buffer* planeVertexBuffer;
      Video::Buffer* planeIndexBuffer;
      Video::GeometryBinding* planeGeometryBinding;
      Video::Texture* planeAlbedo;

      Video::Shader* cubeVertexShader;
      Video::Shader* cubeFragmentShader;
      Video::ShaderProgram* cubeShaderProgram;
      Video::GraphicsPipeline* cubePipeline;

      Meshes::Cube* cube;
      Video::Texture* cubeAlbedo;

      Video::Buffer* cubeInstanceBuffer;
      Video::Buffer* cubeUniformBuffer;
      Video::Buffer* cubeLightBuffer;

      const uint32_t cubeInstanceCount = 5000;

      Video::VertexDescription* offsetVertexDescription;
      Video::Buffer* offsetVertexBuffer;
      Video::Buffer* offsetIndexBuffer;
      Video::GeometryBinding* offsetGeometryBinding;
      Video::Texture* offsetTexture;
      Video::Buffer* offsetInstanceBuffer;

      Video::Buffer* matrixBuffer;
      Video::Buffer* instanceBuffer;
      Video::Buffer* uniformBuffer;
      Video::Buffer* lightBuffer;

      const uint32_t instanceCount = 100;
};
