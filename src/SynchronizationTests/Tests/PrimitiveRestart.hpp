#pragma once

#include "../Framework/SynchronizationTest.hpp"

namespace Video {
    class Texture;
    class CommandBuffer;
    class Shader;
    class ShaderProgram;
    class GraphicsPipeline;
    class VertexDescription;
    class GeometryBinding;
    class Buffer;
}

class PrimitiveRestart : public SynchronizationTest {
public:
    const char* GetName() const override;
    void Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) override;
    void Frame() override;
    void Shutdown() override;

private:
    Video::LowLevelRenderer* renderer;
    Video::Texture* renderTarget;
    Video::Texture* mainRenderTarget;
    Video::CommandBuffer* commandBuffer;

    Video::Shader* testVertexShader;
    Video::Shader* testFragmentShader;
    Video::ShaderProgram* testShaderProgram;
    Video::GraphicsPipeline* testPipeline;
    Video::GraphicsPipeline* testLinePipeline;

    Video::Shader* upscaleVertexShader;
    Video::Shader* upscaleFragmentShader;
    Video::ShaderProgram* upscaleShaderProgram;
    Video::GraphicsPipeline* upscalePipeline;

    Video::Buffer* indexBuffer;
    Video::Buffer* lineIndexBuffer;
    Video::Buffer* triangleIndexBuffer;
    Video::Buffer* matricesBuffer;

    glm::uvec2 screenSize;
};
