#pragma once

#include <glm/glm.hpp>

namespace Video {
class LowLevelRenderer;
class Shader;
class ShaderProgram;
class GraphicsPipeline;
class RenderSurface;
class Filter;
class CommandBuffer;
class Texture;
class RenderPass;
class Buffer;

/// Applies post-processing effects to the rendered image.
class PostProcessing {
  public:
    /// Post-processing configuration.
    struct Configuration {
        float gamma = 2.2f;

        struct FXAA {
            bool enabled = false;
        } fxaa;

        struct Dither {
            bool enabled = false;
        } dither;

        struct Bloom {
            bool enabled = false;
            float intensity = 1.0f;
            float threshold = 1.0f;
            float scatter = 0.7f;
        } bloom;
    };

    /// Create new post-processing handler.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     * @param outputTexture The texture to output to.
     */
    PostProcessing(LowLevelRenderer* lowLevelRenderer, Texture* outputTexture);

    /// Destructor.
    ~PostProcessing();

    /// Set texture to output to.
    /**
     * @param outputTexture The texture to output to.
     */
    void SetOutputTexture(Texture* outputTexture);

    /// Configure the post-processing.
    /**
     * @param configuration The configuration.
     */
    void Configure(const Configuration& configuration);

    /// Apply post-processing effects.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param inputTexture The render texture containing the rendered image.
     *
     * @return The texture to blit to the swap chain.
     */
    void ApplyPostProcessing(CommandBuffer& commandBuffer, Texture* inputTexture);

  private:
    PostProcessing(const PostProcessing& other) = delete;

    void CreateRenderPasses(Texture* outputTexture);
    void FreeRenderPasses();

    void CreateBloomResources();
    void FreeBloomResources();
    void GenerateBloomTexture(CommandBuffer& commandBuffer, Texture* inputTexture);

    LowLevelRenderer* lowLevelRenderer;

    Configuration configuration;
    glm::uvec2 screenSize;
    Shader* vertexShader;

    Texture* tempTexture;
    RenderPass* tempRenderPass;
    RenderPass* outputRenderPass;

    Texture* dummyTexture;

    // Bloom.
    struct BloomData {
        Texture* textures[2];
        RenderPass* renderPasses[2];
    };
    BloomData* bloomPasses;
    uint32_t bloomPassCount;
    Shader* bloomThresholdShader;
    ShaderProgram* bloomThresholdShaderProgram;
    GraphicsPipeline* bloomThresholdPipeline;
    Shader* bloomDownscaleShader;
    ShaderProgram* bloomDownscaleShaderProgram;
    GraphicsPipeline* bloomDownscalePipeline;
    Shader* bloomUpscaleShader;
    ShaderProgram* bloomUpscaleShaderProgram;
    GraphicsPipeline* bloomUpscalePipeline;
    Shader* bloomBlurShader;
    ShaderProgram* bloomBlurShaderProgram;
    GraphicsPipeline* bloomBlurPipeline;
    bool bloomResourcesCreated = false;

    // Uber.
    Shader* uberShader;
    ShaderProgram* uberShaderProgram;
    GraphicsPipeline* uberPipeline;
    Buffer* uberUniformBuffer;

    struct UberUniforms {
        float gamma;
        float bloomIntensity;
        float time;
        uint32_t ditherEnable;
    };

    // FXAA + dither.
    Shader* fxaaShader;
    ShaderProgram* fxaaShaderProgram;
    GraphicsPipeline* fxaaPipeline;
    Buffer* fxaaUniformBuffer;

    struct FXAAUniforms {
        glm::vec2 screenSize;
        float time;
        uint32_t ditherEnable;
    };
};

} // namespace Video
