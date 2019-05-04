#pragma once

#include "../Interface/CommandBuffer.hpp"

#include <vector>
#include <glad/glad.h>

namespace Video {

class OpenGLRenderer;
class OpenGLShaderProgram;

/// OpenGL implementation of CommandBuffer.
class OpenGLCommandBuffer : public CommandBuffer {
  public:
    /// Create new OpenGL command buffer.
    /**
     * @param openGLRenderer The OpenGL renderer.
     */
    explicit OpenGLCommandBuffer(const OpenGLRenderer& openGLRenderer);

    /// Destructor.
    ~OpenGLCommandBuffer() final;

    void BeginRenderPass(RenderPass* renderPass) final;
    void EndRenderPass() final;
    void BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) final;
    void SetViewport(const glm::uvec2& origin, const glm::uvec2& size) final;
    void SetScissor(const glm::uvec2& origin, const glm::uvec2& size) final;
    void SetLineWidth(float width) final;
    void BindGeometry(const GeometryBinding* geometryBinding) final;
    void BindUniformBuffer(ShaderProgram::BindingType bindingType, Buffer* uniformBuffer) final;
    void BindStorageBuffer(Buffer* storageBuffer) final;
    void BindMaterial(std::initializer_list<const Texture*> textures) final;
    void PushConstants(const void* data) final;
    void Draw(unsigned int vertexCount, unsigned int firstVertex) final;
    void DrawIndexed(unsigned int indexCount, unsigned int firstIndex, unsigned int baseVertex) final;
    void BlitToSwapChain(Texture* texture) final;

    /// Submit the commands in the command buffer to the GPU.
    void Submit();

  private:
    OpenGLCommandBuffer(const OpenGLCommandBuffer& other) = delete;

    struct BeginRenderPassCommand {
        GLuint frameBuffer;
        GLbitfield clearMask;
    };

    struct BindGraphicsPipelineCommand {
        GLuint program;
        GLenum polygonMode;
        bool cullFaceEnabled;
        GLenum cullFace;
        bool blendingEnabled;
        GLenum blendingSourceFactor;
        GLenum blendingDestinationFactor;
        bool depthTestEnabled;
        GLboolean depthMaskEnabled;
        GLenum depthFunction;
    };

    struct SetViewportCommand {
        unsigned int origin[2];
        unsigned int size[2];
    };

    struct SetScissorCommand {
        unsigned int origin[2];
        unsigned int size[2];
    };

    struct SetLineWidthCommand {
        float width;
    };

    struct BindGeometryCommand {
        GLuint vertexArray;
    };

    struct SetUniformIntegerCommand {
        unsigned int location;
        int value;
    };

    struct SetUniformFloatCommand {
        unsigned int location;
        float value;
    };

    struct SetUniformVector2Command {
        unsigned int location;
        float value[2];
    };

    struct SetUniformVector3Command {
        unsigned int location;
        float value[3];
    };

    struct SetUniformVector4Command {
        unsigned int location;
        float value[4];
    };

    struct SetUniformMatrix3Command {
        unsigned int location;
        float value[3 * 3];
    };

    struct SetUniformMatrix4Command {
        unsigned int location;
        float value[4 * 4];
    };

    struct BindTextureCommand {
        GLenum slot;
        GLuint texture;
    };

    struct BindUniformBufferCommand {
        GLuint index;
        GLuint uniformBuffer;
    };

    struct BindStorageBufferCommand {
        GLuint index;
        GLuint storageBuffer;
    };

    struct DrawCommand {
        GLenum mode;
        GLint first;
        GLsizei count;
    };

    struct DrawIndexedCommand {
        GLenum mode;
        GLsizei count;
        GLenum type;
        GLvoid* indices;
        GLint baseVertex;
    };

    struct BlitToSwapChainCommand {
        GLuint texture;
    };

    struct Command {
        union {
            BeginRenderPassCommand beginRenderPassCommand;
            BindGraphicsPipelineCommand bindGraphicsPipelineCommand;
            SetViewportCommand setViewportCommand;
            SetScissorCommand setScissorCommand;
            SetLineWidthCommand setLineWidthCommand;
            BindGeometryCommand bindGeometryCommand;
            SetUniformIntegerCommand setUniformIntegerCommand;
            SetUniformFloatCommand setUniformFloatCommand;
            SetUniformVector2Command setUniformVector2Command;
            SetUniformVector3Command setUniformVector3Command;
            SetUniformVector4Command setUniformVector4Command;
            SetUniformMatrix3Command setUniformMatrix3Command;
            SetUniformMatrix4Command setUniformMatrix4Command;
            BindTextureCommand bindTextureCommand;
            BindUniformBufferCommand bindUniformBufferCommand;
            BindStorageBufferCommand bindStorageBufferCommand;
            DrawCommand drawCommand;
            DrawIndexedCommand drawIndexedCommand;
            BlitToSwapChainCommand blitToSwapChainCommand;
        };

        enum class Type {
            BEGIN_RENDER_PASS,
            END_RENDER_PASS,
            BIND_GRAPHICS_PIPELINE,
            SET_VIEWPORT,
            SET_SCISSOR,
            SET_LINE_WIDTH,
            BIND_GEOMETRY,
            SET_UNIFORM_INTEGER,
            SET_UNIFORM_FLOAT,
            SET_UNIFORM_VECTOR2,
            SET_UNIFORM_VECTOR3,
            SET_UNIFORM_VECTOR4,
            SET_UNIFORM_MATRIX3,
            SET_UNIFORM_MATRIX4,
            BIND_TEXTURE,
            BIND_UNIFORM_BUFFER,
            BIND_STORAGE_BUFFER,
            DRAW,
            DRAW_INDEXED,
            BLIT_TO_SWAP_CHAIN
        } type;
    };

    void SetUniformInteger(unsigned int location, int value);
    void SetUniformFloat(unsigned int location, float value);
    void SetUniformVector2(unsigned int location, const glm::vec2& value);
    void SetUniformVector3(unsigned int location, const glm::vec3& value);
    void SetUniformVector4(unsigned int location, const glm::vec4& value);
    void SetUniformMatrix3(unsigned int location, const glm::mat3& value);
    void SetUniformMatrix4(unsigned int location, const glm::mat4& value);

    void AddCommand(const Command& command);
    void SubmitCommand(const Command& command);

    std::vector<Command> commands;
    GLenum primitiveType;
    GLenum indexType;
    uint32_t indexSize;
    uint32_t frameBufferHeight;

    bool inRenderPass = false;

    const OpenGLShaderProgram* currentShaderProgram = nullptr;

    const OpenGLShaderProgram* blitShaderProgram;
};

} // namespace Video
