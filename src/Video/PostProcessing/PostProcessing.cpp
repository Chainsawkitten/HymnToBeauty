#include "PostProcessing.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include "../LowLevelRenderer/Interface/CommandBuffer.hpp"
#include "../LowLevelRenderer/Interface/Shader.hpp"
#include "../LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "../LowLevelRenderer/Interface/Texture.hpp"
#include <chrono>
#include <algorithm>

#include "PostProcessing.vert.hpp"
#include "UberPostProcessing.frag.hpp"
#include "FXAA.frag.hpp"
#include "BloomThreshold.frag.hpp"
#include "BloomDownscale.frag.hpp"
#include "BloomBlur.frag.hpp"
#include "BloomUpscale.frag.hpp"

using namespace Video;

PostProcessing::PostProcessing(LowLevelRenderer* lowLevelRenderer, Texture* outputTexture) {
    this->lowLevelRenderer = lowLevelRenderer;
    this->screenSize = outputTexture->GetSize();

    vertexShader = lowLevelRenderer->CreateShader(POSTPROCESSING_VERT, Shader::Type::VERTEX_SHADER);

    GraphicsPipeline::Configuration configuration = {};
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
    unsigned char dummyData[4] = { 0, 0, 0, 0 };
    dummyTexture = lowLevelRenderer->CreateTexture(glm::uvec2(1, 1), Texture::Type::COLOR, Texture::Format::R8G8B8A8, 4, dummyData);
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

    FreeRenderPasses();
    if (bloomResourcesCreated)
        FreeBloomResources();

    delete vertexShader;

    delete dummyTexture;
}

void PostProcessing::SetOutputTexture(Texture* outputTexture) {
    FreeRenderPasses();
    if (bloomResourcesCreated)
        FreeBloomResources();

    this->screenSize = outputTexture->GetSize();

    CreateRenderPasses(outputTexture);
    CreateBloomResources();
}

void PostProcessing::Configure(const Configuration& configuration) {
    this->configuration = configuration;

    // Update uber uniform buffer.
    UberUniforms uberUniforms;
    uberUniforms.gamma = configuration.gamma;
    uberUniforms.bloomIntensity = configuration.bloom.enabled ? configuration.bloom.intensity : 0.0f;

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
    Texture* bloomTexture = dummyTexture;

    // Bloom.
    if (configuration.bloom.enabled) {
        if (!bloomResourcesCreated)
            CreateBloomResources();

        GenerateBloomTexture(commandBuffer, inputTexture);
        bloomTexture = bloomPasses[0].textures[1];
    }

    // Uber.
    commandBuffer.BeginRenderPass(hasFxaaPass ? tempRenderPass : outputRenderPass, "Uber post-processing");
    commandBuffer.BindGraphicsPipeline(uberPipeline);
    commandBuffer.SetViewportAndScissor(glm::uvec2(0, 0), screenSize);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, uberUniformBuffer);
    commandBuffer.BindMaterial({inputTexture, bloomTexture});
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

void PostProcessing::CreateBloomResources() {
    glm::uvec2 size = screenSize;
    const uint32_t minSize = std::min(size.x, size.y);
    // Only go down to 8x8. But we need at least one upscale pass, so at least 2 passes.
    bloomPassCount = std::max(static_cast<int>(log2(minSize)) - 3, 2);

    bloomPasses = new BloomData[bloomPassCount];
    for (uint32_t i = 0; i < bloomPassCount; i++) {
        size /= 2;

        bloomPasses[i].textures[0] = lowLevelRenderer->CreateTexture(size, Texture::Type::RENDER_COLOR, Texture::Format::R11G11B10);
        bloomPasses[i].textures[1] = lowLevelRenderer->CreateTexture(size, Texture::Type::RENDER_COLOR, Texture::Format::R11G11B10);
        bloomPasses[i].renderPasses[0] = lowLevelRenderer->CreateRenderPass(bloomPasses[i].textures[0]);
        bloomPasses[i].renderPasses[1] = lowLevelRenderer->CreateRenderPass(bloomPasses[i].textures[1]);
    }

    bloomResourcesCreated = true;
}

void PostProcessing::FreeBloomResources() {
    for (uint32_t i = 0; i < bloomPassCount; i++) {
        delete bloomPasses[i].textures[0];
        delete bloomPasses[i].textures[1];
        delete bloomPasses[i].renderPasses[0];
        delete bloomPasses[i].renderPasses[1];
    }
    delete[] bloomPasses;

    bloomResourcesCreated = false;
}

void PostProcessing::GenerateBloomTexture(CommandBuffer& commandBuffer, Texture* inputTexture) {
    // Downscale.
    for (uint32_t i = 0; i < bloomPassCount; i++) {
        const glm::uvec2 size = bloomPasses[i].textures[0]->GetSize();

        if (i == 0) {
            // Threshold.
            commandBuffer.BeginRenderPass(bloomPasses[i].renderPasses[0], "Bloom threshold");
            commandBuffer.BindGraphicsPipeline(bloomThresholdPipeline);
            commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
            commandBuffer.PushConstants(&configuration.bloom.threshold);
            commandBuffer.BindMaterial({inputTexture});
            commandBuffer.Draw(3);
            commandBuffer.EndRenderPass();
        } else {
            // Downscale
            commandBuffer.BeginRenderPass(bloomPasses[i].renderPasses[0], "Bloom downscale");
            commandBuffer.BindGraphicsPipeline(bloomDownscalePipeline);
            commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
            commandBuffer.BindMaterial({bloomPasses[i - 1].textures[0]});
            commandBuffer.Draw(3);
            commandBuffer.EndRenderPass();
        }

        // Horizontal blur.
        commandBuffer.BeginRenderPass(bloomPasses[i].renderPasses[1], "Horizontal blur");
        commandBuffer.BindGraphicsPipeline(bloomBlurPipeline);
        commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
        commandBuffer.BindMaterial({bloomPasses[i].textures[0]});
        glm::vec2 offset(1.0f / size.x, 0.0f);
        commandBuffer.PushConstants(&offset);
        commandBuffer.Draw(3);
        commandBuffer.EndRenderPass();

        // Vertical blur.
        commandBuffer.BeginRenderPass(bloomPasses[i].renderPasses[0], "Vertical blur");
        commandBuffer.BindGraphicsPipeline(bloomBlurPipeline);
        commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
        commandBuffer.BindMaterial({bloomPasses[i].textures[1]});
        offset = glm::vec2(0.0f, 1.0f / size.y);
        commandBuffer.PushConstants(&offset);
        commandBuffer.Draw(3);
        commandBuffer.EndRenderPass();
    }

    // Upscale
    for (uint32_t i = 0; i < bloomPassCount - 1; i++) {
        const uint32_t pass = bloomPassCount - 2 - i;
        const glm::uvec2 size = bloomPasses[pass].textures[0]->GetSize();

        commandBuffer.BeginRenderPass(bloomPasses[pass].renderPasses[1], "Bloom upscale");
        commandBuffer.BindGraphicsPipeline(bloomUpscalePipeline);
        commandBuffer.SetViewportAndScissor(glm::uvec2(0.0f, 0.0f), size);
        commandBuffer.PushConstants(&configuration.bloom.scatter);
        commandBuffer.BindMaterial({bloomPasses[pass].textures[0], bloomPasses[pass + 1].textures[i == 0 ? 0 : 1]});
        commandBuffer.Draw(3);
        commandBuffer.EndRenderPass();
    }
}
