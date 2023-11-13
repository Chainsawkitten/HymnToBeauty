#include "PrimitiveRestart.hpp"

#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/LowLevelRenderer/Interface/Shader.hpp>
#include <Video/LowLevelRenderer/Interface/ShaderProgram.hpp>
#include <Video/LowLevelRenderer/Interface/GraphicsPipeline.hpp>
#include <Video/LowLevelRenderer/Interface/Buffer.hpp>
#include <Video/Geometry/VertexType/StaticVertex.hpp>
#include <Framework/Meshes.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "PostProcessing.vert.hpp"
#include "Sample.frag.hpp"
#include "PrimitiveRestart.vert.hpp"
#include "PrimitiveRestart.frag.hpp"

using namespace Video;

const char* PrimitiveRestart::GetName() const {
    return "Primitive restart";
}

const glm::uvec2 renderSize = glm::uvec2(8, 4);

void PrimitiveRestart::Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) {
    this->renderer = renderer;
    this->screenSize = screenSize;

    // Create index buffers.
    uint32_t indices[] = { 1, 2, 0, 0, 0, 0, 0, 1, 3, 0 };
    indexBuffer = renderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER, sizeof(indices), &indices);

    uint32_t lineIndices[] = { 0, 1, 0xFFFFFFFFu, 2, 3, 0 };
    lineIndexBuffer = renderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER, sizeof(lineIndices), &lineIndices);

    uint32_t triangleIndices[] = { 0, 1, 3, 0xFFFFFFFEu, 2, 1, 0, 0 };
    triangleIndexBuffer = renderer->CreateBuffer(Buffer::BufferUsage::INDEX_BUFFER, sizeof(triangleIndices), &triangleIndices);

    renderTarget = renderer->CreateRenderTarget(renderSize * 3u, Texture::Format::R8);
    mainRenderTarget = renderer->CreateRenderTarget(screenSize, Texture::Format::R8G8B8A8);
    commandBuffer = renderer->CreateCommandBuffer();

    // Matrices buffer.
    const glm::vec2 positions[8] = {
        glm::vec2(0.01f, 0.98f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.99f, -0.98f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.99f, 0.98f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.01f, -0.98f),
        glm::vec2(0.0f, 0.0f)
    };
    matricesBuffer = renderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(positions), positions);

    // Main test pipeline.
    testVertexShader = renderer->CreateShader(PRIMITIVERESTART_VERT, Shader::Type::VERTEX_SHADER);
    testFragmentShader = renderer->CreateShader(PRIMITIVERESTART_FRAG, Shader::Type::FRAGMENT_SHADER);
    testShaderProgram = renderer->CreateShaderProgram({ testVertexShader, testFragmentShader });

    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;

    testPipeline = renderer->CreateGraphicsPipeline(testShaderProgram, configuration);

    configuration.primitiveType = PrimitiveType::LINE;

    testLinePipeline = renderer->CreateGraphicsPipeline(testShaderProgram, configuration);

    // Pipeline to upscale.
    upscaleVertexShader = renderer->CreateShader(POSTPROCESSING_VERT, Shader::Type::VERTEX_SHADER);
    upscaleFragmentShader = renderer->CreateShader(SAMPLE_FRAG, Shader::Type::FRAGMENT_SHADER);
    upscaleShaderProgram = renderer->CreateShaderProgram({ upscaleVertexShader, upscaleFragmentShader });

    configuration.primitiveType = PrimitiveType::TRIANGLE;

    upscalePipeline = renderer->CreateGraphicsPipeline(upscaleShaderProgram, configuration);
}

void PrimitiveRestart::Frame() {
    // Render test.
    commandBuffer->BeginRenderPass(renderTarget, RenderPass::LoadOperation::CLEAR, nullptr, RenderPass::LoadOperation::DONT_CARE, "Test renderpass");
    commandBuffer->BindGraphicsPipeline(testPipeline);

    // Test 1: Triangle list. Regular values.
    //commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), renderSize);
    //commandBuffer->SetIndexBuffer(indexBuffer, GeometryBinding::IndexType::INT, 0);
    //commandBuffer->DrawIndexed(10);
    //
    //// Test 2: Triangle list. Regular values.
    //commandBuffer->SetViewportAndScissor(glm::uvec2(8, 0), renderSize);
    //commandBuffer->SetIndexBuffer(indexBuffer, GeometryBinding::IndexType::INT, 12);
    //commandBuffer->DrawIndexed(7);
    //
    //// Test 3: Triangle list. Regular values.
    //commandBuffer->SetViewportAndScissor(glm::uvec2(16, 0), renderSize);
    //commandBuffer->SetIndexBuffer(indexBuffer, GeometryBinding::IndexType::INT, 36);
    //commandBuffer->DrawIndexed(0);

    // Test 4: Line list. 0xFFFFFFFF
    //commandBuffer->BindGraphicsPipeline(testLinePipeline);
    //commandBuffer->SetViewportAndScissor(glm::uvec2(0, 4), renderSize);
    //commandBuffer->SetIndexBuffer(lineIndexBuffer, GeometryBinding::IndexType::INT, 0);
    //commandBuffer->DrawIndexed(6);

    // Test 5: Triangle list. 0xFFFFFFFF
    commandBuffer->BindGraphicsPipeline(testPipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(8, 4), renderSize);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, matricesBuffer);
    commandBuffer->SetIndexBuffer(triangleIndexBuffer, GeometryBinding::IndexType::INT, 0);
    commandBuffer->DrawIndexed(6);


    commandBuffer->EndRenderPass();

    // Upscale results.
    commandBuffer->BeginRenderPass(mainRenderTarget, RenderPass::LoadOperation::CLEAR);
    commandBuffer->BindGraphicsPipeline(upscalePipeline);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer->BindMaterial({ { renderTarget, renderer->GetSampler(Sampler::Filter::NEAREST, Sampler::Clamping::CLAMP_TO_EDGE) } });
    commandBuffer->Draw(3);
    commandBuffer->EndRenderPass();

    commandBuffer->BlitToSwapChain(mainRenderTarget);

    renderer->Submit(commandBuffer);
}

void PrimitiveRestart::Shutdown() {
    delete testPipeline;
    delete testLinePipeline;
    delete testShaderProgram;
    delete testVertexShader;
    delete testFragmentShader;

    delete upscalePipeline;
    delete upscaleShaderProgram;
    delete upscaleVertexShader;
    delete upscaleFragmentShader;

    delete matricesBuffer;
    delete indexBuffer;
    delete lineIndexBuffer;
    delete triangleIndexBuffer;

    delete mainRenderTarget;
    delete renderTarget;
    delete commandBuffer;
}
