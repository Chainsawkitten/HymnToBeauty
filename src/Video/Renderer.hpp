#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Lighting/Light.hpp"
#include "PostProcessing/PostProcessing.hpp"

struct GLFWwindow;

namespace Video {
class StaticRenderProgram;
class Texture2D;
class Shader;
class ShaderProgram;
class RenderPass;
class Buffer;
class VertexDescription;
class GeometryBinding;
class LowLevelRenderer;
class CommandBuffer;
class Texture;
namespace Geometry {
class Geometry3D;
} // namespace Geometry

/// Handles rendering using a low-level renderer.
class Renderer {
  public:
    /// The supported graphics APIs.
    enum class GraphicsAPI {
        OPENGL, ///< OpenGL
        VULKAN  ///< Vulkan
    };

    /// Create new renderer.
    /**
     * @param graphicsAPI Which graphics API to use for rendering.
     * @param window Window to render to.
     */
    Renderer(GraphicsAPI graphicsAPI, GLFWwindow* window);

    /// Destructor.
    ~Renderer();

    /// Get the low-level renderer.
    /**
     * @return The low-level renderer.
     */
    LowLevelRenderer* GetLowLevelRenderer();

    /// Set the size of the render surface.
    /**
     * @param size The size of the render surface.
     */
    void SetRenderSurfaceSize(const glm::uvec2& size);

    /// Get the size of the render surface.
    /**
     * @return The size of the render surface.
     */
    const glm::uvec2& GetRenderSurfaceSize() const;

    /// Begin new frame.
    void BeginFrame();

    /// Start depth pre-pass.
    void StartDepthPrePass();

    /// Start the main forward shading pass.
    void StartMainPass();

    /// Prepare for depth rendering static meshes.
    /**
     * @param viewMatrix The camera's view matrix.
     * @param projectionMatrix The camera's projection matrix.
     */
    void PrepareStaticMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

    /// Render a static mesh.
    /**
     * @param geometry The geometry to render.
     * @param modelMatrix Model matrix.
     */
    void DepthRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& modelMatrix);

    /// Prepare for rendering static meshes.
    /**
     * @param viewMatrix The camera's view matrix.
     * @param projectionMatrix The camera's projection matrix.
     * @param cameraNear Camera near plane distance.
     * @param cameraFar Camera far plane distance.
     */
    void PrepareStaticMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float cameraNear, float cameraFar);

    /// Render a static mesh.
    /**
     * @param geometry The geometry to render.
     * @param albedo Albedo texture.
     * @param normal Normal map.
     * @param roughnessMetallic Roughness-metallic texture.
     * @param modelMatrix Model matrix.
     */
    void RenderStaticMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* roughnessMetallic, const glm::mat4 modelMatrix);

    /// Update light buffer.
    /**
     * @param lights Vector of lights to push to the light buffer.
     */
    void SetLights(const std::vector<Light>& lights);

    /// Apply post-processing.
    void ApplyPostProcessing();

    /// Display the rendered results to back buffer and swap back and front buffers.
    void Present();

    /// Wait until rendering and presentation is done.
    /** 
     * \note
     * Needs to be done before destroying render resources or they could still be in use.
     */
    void WaitForRender();

    /// Begin rendering icons.
    /**
     * \note
     * Needs to be called before RenderIcon.
     * @param viewProjectionMatrix The camera's view projection matrix.
     * @param cameraPosition The camera's position.
     * @param cameraUp The camera's up vector.
     */
    void PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp);

    /// Render a billboarded icon.
    /**
     * \note
     * PrepareRenderingIcons must be called before.
     * @param position World position to render at.
     * @param icon The icon to render.
     */
    void RenderIcon(const glm::vec3& position, const Texture2D* icon);

    /// Configure the post-processing.
    /**
     * @param configuration The configuration.
     */
    void ConfigurePostProcessing(const PostProcessing::Configuration& configuration);

    /// Gamma correction.
    /**
     * @param gamma Gamma to apply.
     */
    void SetGamma(float gamma);

    /// Gamma correction.
    /**
     * @return gamma Gamma value.
     */
    float GetGamma() const;

    /// Get the command buffer where commands are being recorded.
    /**
     * @return The command buffer to record into.
     */
    CommandBuffer* GetCommandBuffer();

  private:
    Renderer(const Renderer& other) = delete;

    void CreateRenderTextures();
    void FreeRenderTextures();

    LowLevelRenderer* lowLevelRenderer;

    glm::uvec2 renderSurfaceSize;
    Texture* colorTexture;
    Texture* depthTexture;
    Texture* postProcessingTexture;
    RenderPass* depthRenderPass;
    RenderPass* mainRenderPass;
    RenderPass* iconRenderPass;
    CommandBuffer* commandBuffer;

    StaticRenderProgram* staticRenderProgram;

    static const uint32_t MAX_LIGHTS = 100;
    unsigned int lightCount;
    Buffer* lightBuffer;
    Light lightData[MAX_LIGHTS];

    PostProcessing* postProcessing;

    // Icon rendering.
    Shader* iconVertexShader;
    Shader* iconFragmentShader;
    ShaderProgram* iconShaderProgram;
    GraphicsPipeline* iconGraphicsPipeline;

    Buffer* iconVertexBuffer;
    VertexDescription* iconVertexDescription;
    GeometryBinding* iconGeometryBinding;
    const Texture2D* currentIcon = nullptr;
    Buffer* iconMatricesBuffer;

    glm::vec3 cameraPosition;
    glm::vec3 cameraUp;
};
} // namespace Video
