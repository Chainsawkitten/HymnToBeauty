#include "PostProcessing.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include "../LowLevelRenderer/Interface/CommandBuffer.hpp"
#include "../LowLevelRenderer/Interface/Shader.hpp"
#include "../LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "../LowLevelRenderer/Interface/Texture.hpp"
#include <chrono>

#include "PostProcessing.vert.hpp"
#include "UberPostProcessing.frag.hpp"
#include "FXAA.frag.hpp"

using namespace Video;

PostProcessing::PostProcessing(LowLevelRenderer* lowLevelRenderer, Texture* outputTexture) {
    this->lowLevelRenderer = lowLevelRenderer;
    this->screenSize = outputTexture->GetSize();

    vertexShader = lowLevelRenderer->CreateShader(POSTPROCESSING_VERT, Shader::Type::VERTEX_SHADER);

    GraphicsPipeline::Configuration configuration;
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;
    configuration.depthComparison = DepthComparison::ALWAYS;

    // Create textures and renderpasses.
    CreateRenderPasses(outputTexture);

    // Uber.
    uberShader = lowLevelRenderer->CreateShader(UBERPOSTPROCESSING_FRAG, Shader::Type::FRAGMENT_SHADER);
    uberShaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, uberShader});
    uberPipeline = lowLevelRenderer->CreateGraphicsPipeline(uberShaderProgram, configuration);

    uberUniformBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(UberUniforms));

    // FXAA + dither.
    fxaaShader = lowLevelRenderer->CreateShader(FXAA_FRAG, Shader::Type::FRAGMENT_SHADER);
    fxaaShaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, fxaaShader});
    fxaaPipeline = lowLevelRenderer->CreateGraphicsPipeline(fxaaShaderProgram, configuration);

    fxaaUniformBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(FXAAUniforms));
}

PostProcessing::~PostProcessing() {
    delete uberUniformBuffer;
    delete uberPipeline;
    delete uberShaderProgram;
    delete uberShader;

    delete fxaaUniformBuffer;
    delete fxaaPipeline;
    delete fxaaShaderProgram;
    delete fxaaShader;

    FreeRenderPasses();

    delete vertexShader;
}

void PostProcessing::SetOutputTexture(Texture* outputTexture) {
    FreeRenderPasses();

    this->screenSize = outputTexture->GetSize();

    CreateRenderPasses(outputTexture);
}

void PostProcessing::Configure(const Configuration& configuration) {
    this->configuration = configuration;

    // Update uber uniform buffer.
    UberUniforms uberUniforms;
    uberUniforms.gamma = configuration.gamma;

    uberUniformBuffer->Write(&uberUniforms);

    // Update FXAA uniform buffer.
    FXAAUniforms fxaaUniforms;
    fxaaUniforms.screenSize = screenSize;
    float time = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() % 30000000000 / 1000000000.0);
    fxaaUniforms.time = time;
    fxaaUniforms.fxaaEnable = configuration.fxaa.enabled;
    fxaaUniforms.ditherEnable = configuration.dither.enabled;

    fxaaUniformBuffer->Write(&fxaaUniforms);
}

void PostProcessing::ApplyPostProcessing(CommandBuffer& commandBuffer, Texture* inputTexture) {
    bool hasFxaaPass = configuration.fxaa.enabled || configuration.dither.enabled;

    // Uber.
    commandBuffer.BeginRenderPass(hasFxaaPass ? tempRenderPass : outputRenderPass, "Uber post-processing");
    commandBuffer.BindGraphicsPipeline(uberPipeline);
    commandBuffer.SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uberUniformBuffer);
    commandBuffer.BindMaterial({inputTexture});
    commandBuffer.Draw(3);

    // FXAA + dither.
    if (hasFxaaPass) {
        commandBuffer.EndRenderPass();

        commandBuffer.BeginRenderPass(outputRenderPass, "FXAA");
        commandBuffer.BindGraphicsPipeline(fxaaPipeline);
        commandBuffer.SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
        commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, fxaaUniformBuffer);
        commandBuffer.BindMaterial({tempTexture});
        commandBuffer.Draw(3);
    }
}

void PostProcessing::CreateRenderPasses(Texture* outputTexture) {
    tempTexture = lowLevelRenderer->CreateTexture(outputTexture->GetSize(), Texture::Type::RENDER_COLOR, Texture::Format::R11G11B10);

    tempRenderPass = lowLevelRenderer->CreateRenderPass(tempTexture, RenderPass::LoadOperation::DONT_CARE);
    outputRenderPass = lowLevelRenderer->CreateRenderPass(outputTexture, RenderPass::LoadOperation::DONT_CARE);
}

void PostProcessing::FreeRenderPasses() {
    delete tempRenderPass;
    delete outputRenderPass;

    delete tempTexture;
}
