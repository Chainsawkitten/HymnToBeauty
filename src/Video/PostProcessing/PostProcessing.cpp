#include "PostProcessing.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include "../LowLevelRenderer/Interface/CommandBuffer.hpp"
#include "../LowLevelRenderer/Interface/Shader.hpp"
#include "../LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "../LowLevelRenderer/Interface/Texture.hpp"
#include <chrono>
#include <algorithm>
#include <cassert>

#include "PostProcessing.vert.hpp"
#include "UberPostProcessing.frag.hpp"
#include "FXAA.frag.hpp"
#include "BloomThreshold.frag.hpp"
#include "BloomDownscale.frag.hpp"
#include "BloomBlur.frag.hpp"
#include "BloomUpscale.frag.hpp"

using namespace Video;

PostProcessing::PostProcessing(LowLevelRenderer* lowLevelRenderer) {
    this->lowLevelRenderer = lowLevelRenderer;

    vertexShader = lowLevelRenderer->CreateShader(POSTPROCESSING_VERT, Shader::Type::VERTEX_SHADER);

    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;
    configuration.depthComparison = DepthComparison::ALWAYS;

    // Uber.
    uberShader = lowLevelRenderer->CreateShader(UBERPOSTPROCESSING_FRAG, Shader::Type::FRAGMENT_SHADER);
    uberShaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, uberShader});
    uberPipeline = lowLevelRenderer->CreateGraphicsPipeline(uberShaderProgram, configuration);

    // FXAA.
    fxaaShader = lowLevelRenderer->CreateShader(FXAA_FRAG, Shader::Type::FRAGMENT_SHADER);
    fxaaShaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, fxaaShader});
    fxaaPipeline = lowLevelRenderer->CreateGraphicsPipeline(fxaaShaderProgram, configuration);

    // Bloom.
    bloomThresholdShader = lowLevelRenderer->CreateShader(BLOOMTHRESHOLD_FRAG, Shader::Type::FRAGMENT_SHADER);
    bloomThresholdShaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, bloomThresholdShader});
    bloomThresholdPipeline = lowLevelRenderer->CreateGraphicsPipeline(bloomThresholdShaderProgram, configuration);

    bloomDownscaleShader = lowLevelRenderer->CreateShader(BLOOMDOWNSCALE_FRAG, Shader::Type::FRAGMENT_SHADER);
    bloomDownscaleShaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, bloomDownscaleShader});
    bloomDownscalePipeline = lowLevelRenderer->CreateGraphicsPipeline(bloomDownscaleShaderProgram, configuration);

    bloomBlurShader = lowLevelRenderer->CreateShader(BLOOMBLUR_FRAG, Shader::Type::FRAGMENT_SHADER);
    bloomBlurShaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, bloomBlurShader});
    bloomBlurPipeline = lowLevelRenderer->CreateGraphicsPipeline(bloomBlurShaderProgram, configuration);

    bloomUpscaleShader = lowLevelRenderer->CreateShader(BLOOMUPSCALE_FRAG, Shader::Type::FRAGMENT_SHADER);
    bloomUpscaleShaderProgram = lowLevelRenderer->CreateShaderProgram({vertexShader, bloomUpscaleShader});
    bloomUpscalePipeline = lowLevelRenderer->CreateGraphicsPipeline(bloomUpscaleShaderProgram, configuration);

    // Dummy texture (used to fill texture slots for disabled effects).
    unsigned char dummyData[4] = {0, 0, 0, 0};
    dummyTexture = lowLevelRenderer->CreateTexture(glm::uvec2(1, 1), Texture::Format::R8G8B8A8, 4, dummyData);

    sampler = lowLevelRenderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE);
}

PostProcessing::~PostProcessing() {
    delete uberPipeline;
    delete uberShaderProgram;
    delete uberShader;

    delete fxaaPipeline;
    delete fxaaShaderProgram;
    delete fxaaShader;

    delete bloomThresholdPipeline;
    delete bloomThresholdShaderProgram;
    delete bloomThresholdShader;

    delete bloomDownscalePipeline;
    delete bloomDownscaleShaderProgram;
    delete bloomDownscaleShader;

    delete bloomBlurPipeline;
    delete bloomBlurShaderProgram;
    delete bloomBlurShader;

    delete bloomUpscalePipeline;
    delete bloomUpscaleShaderProgram;
    delete bloomUpscaleShader;

    delete vertexShader;

    delete dummyTexture;
}

void PostProcessing::Configure(const Configuration& configuration, Texture* outputTexture) {
    assert(outputTexture != nullptr);
    assert(outputTexture->GetType() == Texture::Type::RENDER_COLOR);

    this->configuration = configuration;
    this->outputTexture = outputTexture;
    screenSize = outputTexture->GetSize();

    float time = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() % 30000000000 / 1000000000.0);

    // Update uber uniform buffer.
    UberUniforms uberUniforms;
    uberUniforms.gamma = configuration.gamma;
    uberUniforms.bloomIntensity = configuration.bloom.enabled ? configuration.bloom.intensity : 0.0f;
    uberUniforms.time = time;
    // Perform dither in uber pass if there is no FXAA pass.
    uberUniforms.ditherEnable = configuration.dither.enabled && !configuration.fxaa.enabled;

    uberUniformBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(UberUniforms), &uberUniforms);

    // Update FXAA uniform buffer.
    if (configuration.fxaa.enabled) {
        FXAAUniforms fxaaUniforms;
        fxaaUniforms.screenSize = screenSize;
        fxaaUniforms.time = time;
        fxaaUniforms.ditherEnable = configuration.dither.enabled;

        fxaaUniformBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(FXAAUniforms), &fxaaUniforms);
    }
}

void PostProcessing::ApplyPostProcessing(CommandBuffer& commandBuffer, Texture* inputTexture) {
    assert(inputTexture != nullptr);
    assert(outputTexture != nullptr);

    bool hasFxaaPass = configuration.fxaa.enabled;
    Texture* bloomTexture = dummyTexture;

    // Bloom.
    if (configuration.bloom.enabled) {
        CreateBloomResources();
        GenerateBloomTexture(commandBuffer, inputTexture);
        bloomTexture = bloomPasses[0].textures[1];
    }

    // Uber.
    Texture* tempTexture;
    if (hasFxaaPass) {
        tempTexture = lowLevelRenderer->CreateRenderTarget(screenSize, Texture::Format::R11G11B10);
    }
    commandBuffer.BeginRenderPass(hasFxaaPass ? tempTexture : outputTexture, RenderPass::LoadOperation::DONT_CARE, nullptr, RenderPass::LoadOperation::DONT_CARE, "Uber post-processing");
    commandBuffer.BindGraphicsPipeline(uberPipeline);
    commandBuffer.SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uberUniformBuffer);
    commandBuffer.BindMaterial({
        {inputTexture, sampler},
        {bloomTexture, sampler}
    });
    commandBuffer.Draw(3);

    if (configuration.bloom.enabled) {
        FreeBloomResources();
    }

    // FXAA.
    if (hasFxaaPass) {
        commandBuffer.EndRenderPass();

        commandBuffer.BeginRenderPass(outputTexture, RenderPass::LoadOperation::DONT_CARE, nullptr, RenderPass::LoadOperation::DONT_CARE, "FXAA");
        commandBuffer.BindGraphicsPipeline(fxaaPipeline);
        commandBuffer.SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
        commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, fxaaUniformBuffer);
        commandBuffer.BindMaterial({{tempTexture, sampler}});
        commandBuffer.Draw(3);

        lowLevelRenderer->FreeRenderTarget(tempTexture);
    }
}

void PostProcessing::CreateBloomResources() {
    glm::uvec2 size = screenSize;
    const uint32_t minSize = std::min(size.x, size.y);
    // Only go down to 8x8. But we need at least one upscale pass, so at least 2 passes.
    bloomPassCount = std::max(static_cast<int>(log2(minSize)) - 3, 2);

    bloomPasses = new BloomData[bloomPassCount];
    for (uint32_t i = 0; i < bloomPassCount; i++) {
        size /= 2;

        bloomPasses[i].textures[0] = lowLevelRenderer->CreateRenderTarget(size, Texture::Format::R11G11B10);
        bloomPasses[i].textures[1] = lowLevelRenderer->CreateRenderTarget(size, Texture::Format::R11G11B10);
    }
}

void PostProcessing::FreeBloomResources() {
    for (uint32_t i = 0; i < bloomPassCount; i++) {
        lowLevelRenderer->FreeRenderTarget(bloomPasses[i].textures[0]);
        lowLevelRenderer->FreeRenderTarget(bloomPasses[i].textures[1]);
    }
    delete[] bloomPasses;
}

void PostProcessing::GenerateBloomTexture(CommandBuffer& commandBuffer, Texture* inputTexture) {
    // Downscale.
    for (uint32_t i = 0; i < bloomPassCount; i++) {
        const glm::uvec2 size = bloomPasses[i].textures[0]->GetSize();

        if (i == 0) {
            // Threshold.
            commandBuffer.BeginRenderPass(bloomPasses[i].textures[0], RenderPass::LoadOperation::DONT_CARE, nullptr, RenderPass::LoadOperation::DONT_CARE, "Bloom threshold");
            commandBuffer.BindGraphicsPipeline(bloomThresholdPipeline);
            commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
            commandBuffer.PushConstants(&configuration.bloom.threshold);
            commandBuffer.BindMaterial({{inputTexture, sampler}});
            commandBuffer.Draw(3);
            commandBuffer.EndRenderPass();
        } else {
            // Downscale
            commandBuffer.BeginRenderPass(bloomPasses[i].textures[0], RenderPass::LoadOperation::DONT_CARE, nullptr, RenderPass::LoadOperation::DONT_CARE, "Bloom downscale");
            commandBuffer.BindGraphicsPipeline(bloomDownscalePipeline);
            commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
            commandBuffer.BindMaterial({{bloomPasses[i - 1].textures[0], sampler}});
            commandBuffer.Draw(3);
            commandBuffer.EndRenderPass();
        }

        // Horizontal blur.
        commandBuffer.BeginRenderPass(bloomPasses[i].textures[1], RenderPass::LoadOperation::DONT_CARE, nullptr, RenderPass::LoadOperation::DONT_CARE, "Horizontal blur");
        commandBuffer.BindGraphicsPipeline(bloomBlurPipeline);
        commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
        commandBuffer.BindMaterial({{bloomPasses[i].textures[0], sampler}});
        glm::vec2 offset(1.0f / size.x, 0.0f);
        commandBuffer.PushConstants(&offset);
        commandBuffer.Draw(3);
        commandBuffer.EndRenderPass();

        // Vertical blur.
        commandBuffer.BeginRenderPass(bloomPasses[i].textures[0], RenderPass::LoadOperation::DONT_CARE, nullptr, RenderPass::LoadOperation::DONT_CARE, "Vertical blur");
        commandBuffer.BindGraphicsPipeline(bloomBlurPipeline);
        commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
        commandBuffer.BindMaterial({{bloomPasses[i].textures[1], sampler}});
        offset = glm::vec2(0.0f, 1.0f / size.y);
        commandBuffer.PushConstants(&offset);
        commandBuffer.Draw(3);
        commandBuffer.EndRenderPass();
    }

    // Upscale
    for (uint32_t i = 0; i < bloomPassCount - 1; i++) {
        const uint32_t pass = bloomPassCount - 2 - i;
        const glm::uvec2 size = bloomPasses[pass].textures[0]->GetSize();

        commandBuffer.BeginRenderPass(bloomPasses[pass].textures[1], RenderPass::LoadOperation::DONT_CARE, nullptr, RenderPass::LoadOperation::DONT_CARE, "Bloom upscale");
        commandBuffer.BindGraphicsPipeline(bloomUpscalePipeline);
        commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
        commandBuffer.PushConstants(&configuration.bloom.scatter);
        commandBuffer.BindMaterial({
            {bloomPasses[pass].textures[0], sampler},
            {bloomPasses[pass + 1].textures[i == 0 ? 0 : 1], sampler}
        });
        commandBuffer.Draw(3);
        commandBuffer.EndRenderPass();
    }
}
