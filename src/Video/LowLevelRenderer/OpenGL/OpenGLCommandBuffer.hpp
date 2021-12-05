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
    /// Timing for a block of work.
    struct Timing {
        /// The name of the block of work.
        std::string name;

        /// The timestamp for starting the block of work.
        GLuint startQuery;

        /// The timestamp for ending the block of work.
        GLuint endQuery;
    };

    /// Create new OpenGL command buffer.
    /**
     * @param openGLRenderer The OpenGL renderer.
     */
    explicit OpenGLCommandBuffer(OpenGLRenderer& openGLRenderer);

    /// Destructor.
    ~OpenGLCommandBuffer() final;

    void BeginRenderPass(RenderPass* renderPass, const std::string& name) final;
    void EndRenderPass() final;
    void BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) final;
    void SetViewport(const glm::uvec2& origin, const glm::uvec2& size) final;
    void SetScissor(const glm::uvec2& origin, const glm::uvec2& size) final;
    void SetLineWidth(float width) final;
    void BindGeometry(GeometryBinding* geometryBinding) final;
    void BindUniformBuffer(ShaderProgram::BindingType bindingType, Buffer* uniformBuffer) final;
    void BindStorageBuffers(std::initializer_list<Buffer*> buffers) final;
    void BindMaterial(std::initializer_list<Texture*> textures) final;
    void PushConstants(const void* data) final;
    void Draw(unsigned int vertexCount, unsigned int firstVertex) final;
    void DrawIndexed(unsigned int indexCount, unsigned int firstIndex, unsigned int baseVertex) final;
    void DrawIndexedInstanced(unsigned int indexCount, unsigned int instanceCount, unsigned int firstIndex, unsigned int baseVertex) final;
    void BlitToSwapChain(Texture* texture) final;
    void BindComputePipeline(ComputePipeline* computePipeline) final;
    void Dispatch(const glm::uvec3& numGroups, const std::string& name) final;
    void ClearBuffer(Buffer* buffer) final;

    /// Submit the commands in the command buffer to the GPU.
    void Submit();

    /// Get the all timings in the command buffer.
    /**
     * @return All recorded timings.
     */
    const std::vector<Timing>& GetTimings() const;

  private:
    OpenGLCommandBuffer(const OpenGLCommandBuffer& other) = delete;

    struct BeginRenderPassCommand {
        GLuint frameBuffer;
        GLbitfield clearMask;
        unsigned int timingIndex;
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
        bool depthClampEnabled;
        bool conservativeRasterizationEnabled;
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
        int32_t value;
    };

    struct SetUniformUnsignedIntegerCommand {
        unsigned int location;
        uint32_t value;
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
        GLintptr offset;
        GLsizeiptr size;
    };

    struct BindStorageBufferCommand {
        GLuint index;
        GLuint storageBuffer;
        GLintptr offset;
        GLsizeiptr size;
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

    struct DrawIndexedInstancedCommand {
        GLenum mode;
        GLsizei count;
        GLenum type;
        GLvoid* indices;
        GLsizei instanceCount;
        GLint baseVertex;
    };

    struct BlitToSwapChainCommand {
        GLuint texture;
    };

    struct BindComputePipelineCommand {
        GLuint program;
    };

    struct DispatchCommand {
        GLuint groupsX;
        GLuint groupsY;
        GLuint groupsZ;
        unsigned int timingIndex;
    };

    struct ClearBufferCommand {
        GLenum target;
        GLuint buffer;
        GLintptr offset;
        GLsizeiptr size;
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
            SetUniformUnsignedIntegerCommand setUniformUnsignedIntegerCommand;
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
            DrawIndexedInstancedCommand drawIndexedInstancedCommand;
            BlitToSwapChainCommand blitToSwapChainCommand;
            BindComputePipelineCommand bindComputePipelineCommand;
            DispatchCommand dispatchCommand;
            ClearBufferCommand clearBufferCommand;
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
            SET_UNIFORM_UNSIGNED_INTEGER,
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
            DRAW_INDEXED_INSTANCED,
            BLIT_TO_SWAP_CHAIN,
            BIND_COMPUTE_PIPELINE,
            DISPATCH,
            CLEAR_BUFFER,
            MEMORY_BARRIER
        } type;
    };

    void SetUniformInteger(unsigned int location, int32_t value);
    void SetUniformUnsignedInteger(unsigned int location, uint32_t value);
    void SetUniformFloat(unsigned int location, float value);
    void SetUniformVector2(unsigned int location, const glm::vec2& value);
    void SetUniformVector3(unsigned int location, const glm::vec3& value);
    void SetUniformVector4(unsigned int location, const glm::vec4& value);
    void SetUniformMatrix3(unsigned int location, const glm::mat3& value);
    void SetUniformMatrix4(unsigned int location, const glm::mat4& value);

    void AddCommand(const Command& command);
    void SubmitCommand(const Command& command);

    OpenGLRenderer& openGLRenderer;

    std::vector<Command> commands;
    GLenum primitiveType;
    GLenum indexType;
    uint32_t indexSize;
    uint32_t frameBufferHeight;
    uintptr_t indexBufferOffset;

    bool inRenderPass = false;

    const OpenGLShaderProgram* currentShaderProgram = nullptr;

    const OpenGLShaderProgram* blitShaderProgram;

    bool writesStorageBuffer = false;

    std::vector<Timing> timings;
    unsigned int timingIndex;
};

} // namespace Video
