#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Lighting/Light.hpp"
#include "PostProcessing/PostProcessing.hpp"
#include "RenderScene.hpp"

struct GLFWwindow;

namespace Video {
class StaticRenderProgram;
class SpriteRenderProgram;
class Shader;
class ShaderProgram;
class Buffer;
class VertexDescription;
class GeometryBinding;
class LowLevelRenderer;
class CommandBuffer;
class Texture;
class Sampler;
class ZBinning;
class DebugDrawing;
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

    /// Set the size of the output image.
    /**
     * @param size The size of the output image.
     */
    void SetOutputSize(const glm::uvec2& size);

    /// Get the size of the render surface.
    /**
     * @return The size of the render surface.
     */
    const glm::uvec2& GetRenderSurfaceSize() const;

    /// Begin new frame.
    void BeginFrame();

    /// Render a scene.
    /**
     * @param renderScene The scene to render.
     */
    void Render(const RenderScene& renderScene);

    /// Display the rendered results to back buffer and swap back and front buffers.
    void Present();

    /// Wait until rendering and presentation is done.
    /**
     * \note
     * Needs to be done before destroying render resources or they could still be in use.
     */
    void WaitForRender();

    /// Get the command buffer where commands are being recorded.
    /**
     * @return The command buffer to record into.
     */
    CommandBuffer* GetCommandBuffer();

  private:
    Renderer(const Renderer& other) = delete;

    void SetRenderSurfaceSize(const glm::uvec2& size);
    void UpdateLights(const RenderScene& renderScene, const RenderScene::Camera& camera);
    std::vector<std::size_t> FrustumCulling(const RenderScene& renderScene, const RenderScene::Camera& camera);
    void RenderDepthPrePass(const RenderScene& renderScene, const std::vector<std::size_t>& culledMeshes, const RenderScene::Camera& camera);
    void RenderOpaques(const RenderScene& renderScene, const std::vector<std::size_t>& culledMeshes, const RenderScene::Camera& camera);
    void RenderDebugShapes(const RenderScene& renderScene, const RenderScene::Camera& camera);
    void RenderIcons(const RenderScene& renderScene, const RenderScene::Camera& camera);
    void PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp);
    void RenderIcon(const glm::vec3& position);
    void RenderSprites(const RenderScene& renderScene, const RenderScene::Camera& camera);

    LowLevelRenderer* lowLevelRenderer;

    glm::uvec2 outputSize;
    glm::uvec2 renderSurfaceSize;
    Texture* outputTexture;
    Texture* colorTexture;
    Texture* depthTexture;
    Texture* postProcessingTexture;
    CommandBuffer* commandBuffer;

    StaticRenderProgram* staticRenderProgram;
    SpriteRenderProgram* spriteRenderProgram;

    ZBinning* zBinning;

    PostProcessing* postProcessing;

    DebugDrawing* debugDrawing;

    // Blitting (viewport).
    Shader* blitVertexShader;
    Shader* blitFragmentShader;
    ShaderProgram* blitShaderProgram;
    GraphicsPipeline* blitGraphicsPipeline;

    Buffer* quadVertexBuffer;
    VertexDescription* quadVertexDescription;
    GeometryBinding* quadGeometryBinding;

    const Sampler* sampler;

    // Icon rendering.
    Shader* iconVertexShader;
    Shader* iconFragmentShader;
    ShaderProgram* iconShaderProgram;
    GraphicsPipeline* iconGraphicsPipeline;

    glm::vec3 cameraPosition;
    glm::vec3 cameraUp;
};
} // namespace Video
