#pragma once

#include <glm/glm.hpp>
#include "RenderProgram.hpp"

namespace Video {
class LowLevelRenderer;
class Texture2D;
class Shader;
class ShaderProgram;
class GraphicsPipeline;
class Buffer;
class CommandBuffer;
class Texture;
class GeometryBinding;
class VertexDescription;

/// Render program to render a sprite.
class SpriteRenderProgram : public RenderProgram {
  public:
    /// Create new sprite render program.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     */
    explicit SpriteRenderProgram(LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    ~SpriteRenderProgram();

    /// Bind render program.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param viewProjectionMatrix The camera's view projection matrix.
     */
    void PreRender(CommandBuffer& commandBuffer, const glm::mat4& viewProjectionMatrix);

    /// Render sprite.
    /**
     * @param commandBuffer Command buffer to build commands into.
     * @param texture The sprite's texture.
     * @param size The size of the sprite.
     * @param pivot The pivot of the sprite.
     * @param modelMatrix Model matrix.
     * @param tint Color to blend the sprite with.
     */
    void Render(CommandBuffer& commandBuffer, Video::Texture2D* texture, const glm::vec2& size, const glm::vec2& pivot, const glm::mat4& modelMatrix, const glm::vec4& tint) const;

  private:
    SpriteRenderProgram(const SpriteRenderProgram& other) = delete;

    LowLevelRenderer* lowLevelRenderer;

    Buffer* vertexBuffer;
    VertexDescription* vertexDescription;
    GeometryBinding* geometryBinding;

    Shader* vertexShader;
    Shader* fragmentShader;
    ShaderProgram* shaderProgram;
    GraphicsPipeline* graphicsPipeline;

    struct MatricesValues {
        glm::mat4 viewProjectionMatrix;
    };

    struct FragmentUniforms {
        float gamma;
    };
};
} // namespace Video
