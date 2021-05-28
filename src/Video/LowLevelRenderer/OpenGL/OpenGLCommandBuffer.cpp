#include "OpenGLCommandBuffer.hpp"

#include <assert.h>
#include <Utility/Log.hpp>
#include <cstring>
#include <glm/glm.hpp>
#include "OpenGLRenderer.hpp"
#include "OpenGLShaderProgram.hpp"
#include "OpenGLVertexDescription.hpp"
#include "OpenGLGeometryBinding.hpp"
#include "OpenGLTexture.hpp"
#include "OpenGLBuffer.hpp"
#include "OpenGLRenderPass.hpp"
#include "OpenGLGraphicsPipeline.hpp"
#include "OpenGLComputePipeline.hpp"

namespace Video {

OpenGLCommandBuffer::OpenGLCommandBuffer(OpenGLRenderer& openGLRenderer) : openGLRenderer(openGLRenderer) {
    blitShaderProgram = openGLRenderer.GetBlitShaderProgram();
}

OpenGLCommandBuffer::~OpenGLCommandBuffer() {}

void OpenGLCommandBuffer::BeginRenderPass(RenderPass* renderPass, const std::string& name) {
    assert(!inRenderPass);
    assert(renderPass != nullptr);

    frameBufferHeight = static_cast<OpenGLRenderPass*>(renderPass)->GetSize().y;

    Command command = {};
    command.type = Command::Type::BEGIN_RENDER_PASS;

    command.beginRenderPassCommand.frameBuffer = static_cast<OpenGLRenderPass*>(renderPass)->GetFrameBuffer();
    command.beginRenderPassCommand.clearMask = static_cast<OpenGLRenderPass*>(renderPass)->GetClearMask();
    command.beginRenderPassCommand.timingIndex = timings.size();

    AddCommand(command);

    // Timing.
    if (openGLRenderer.IsProfiling()) {
        Timing timing;
        timing.name = name;
        timing.startQuery = openGLRenderer.GetFreeQuery();
        timing.endQuery = openGLRenderer.GetFreeQuery();
        timings.push_back(timing);
    }

    inRenderPass = true;
}

void OpenGLCommandBuffer::EndRenderPass() {
    assert(inRenderPass);

    Command command = {};
    command.type = Command::Type::END_RENDER_PASS;

    AddCommand(command);

    // Add a memory barrier if any of the shaders have written to a storage buffer.
    if (writesStorageBuffer) {
        command.type = Command::Type::MEMORY_BARRIER;
        AddCommand(command);
        writesStorageBuffer = false;
    }

    inRenderPass = false;
}

void OpenGLCommandBuffer::BindGraphicsPipeline(GraphicsPipeline* graphicsPipeline) {
    assert(inRenderPass);
    assert(graphicsPipeline != nullptr);

    Command command = {};
    command.type = Command::Type::BIND_GRAPHICS_PIPELINE;

    OpenGLGraphicsPipeline* openGLGraphicsPipeline = static_cast<OpenGLGraphicsPipeline*>(graphicsPipeline);

    GraphicsPipeline::Configuration configuration = openGLGraphicsPipeline->GetConfiguration();

    currentShaderProgram = openGLGraphicsPipeline->GetShaderProgram();
    command.bindGraphicsPipelineCommand.program = currentShaderProgram->GetID();

    if (currentShaderProgram->WritesToStorageBuffer())
        writesStorageBuffer = true;

    // The type of primitives to render.
    switch (configuration.primitiveType) {
    case PrimitiveType::POINT:
        primitiveType = GL_POINTS;
        break;
    case PrimitiveType::LINE:
        primitiveType = GL_LINES;
        break;
    case PrimitiveType::TRIANGLE:
        primitiveType = GL_TRIANGLES;
        break;
    }

    // Polygon mode.
    switch (configuration.polygonMode) {
    case PolygonMode::FILL:
        command.bindGraphicsPipelineCommand.polygonMode = GL_FILL;
        break;
    case PolygonMode::LINE:
        command.bindGraphicsPipelineCommand.polygonMode = GL_LINE;
        break;
    }

    // Cull face.
    command.bindGraphicsPipelineCommand.cullFaceEnabled = true;

    switch (configuration.cullFace) {
    case CullFace::NONE:
        command.bindGraphicsPipelineCommand.cullFaceEnabled = false;
        break;
    case CullFace::BACK:
        command.bindGraphicsPipelineCommand.cullFace = GL_BACK;
        break;
    case CullFace::FRONT:
        command.bindGraphicsPipelineCommand.cullFace = GL_FRONT;
        break;
    }

    // Depth testing.
    switch (configuration.depthMode) {
    case DepthMode::NONE:
        command.bindGraphicsPipelineCommand.depthTestEnabled = false;
        command.bindGraphicsPipelineCommand.depthMaskEnabled = GL_FALSE;
        break;
    case DepthMode::TEST:
        command.bindGraphicsPipelineCommand.depthTestEnabled = true;
        command.bindGraphicsPipelineCommand.depthMaskEnabled = GL_FALSE;
        break;
    case DepthMode::TEST_WRITE:
        command.bindGraphicsPipelineCommand.depthTestEnabled = true;
        command.bindGraphicsPipelineCommand.depthMaskEnabled = GL_TRUE;
        break;
    }

    // Depth comparison.
    switch (configuration.depthComparison) {
    case DepthComparison::LESS:
        command.bindGraphicsPipelineCommand.depthFunction = GL_LESS;
        break;
    case DepthComparison::LESS_EQUAL:
        command.bindGraphicsPipelineCommand.depthFunction = GL_LEQUAL;
        break;
    case DepthComparison::ALWAYS:
        command.bindGraphicsPipelineCommand.depthFunction = GL_ALWAYS;
        break;
    }

    // Blending.
    switch (configuration.blendMode) {
    case BlendMode::NONE:
        command.bindGraphicsPipelineCommand.blendingEnabled = false;
        break;
    case BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA:
        command.bindGraphicsPipelineCommand.blendingEnabled = true;
        command.bindGraphicsPipelineCommand.blendingSourceFactor = GL_SRC_ALPHA;
        command.bindGraphicsPipelineCommand.blendingDestinationFactor = GL_ONE_MINUS_SRC_ALPHA;
        break;
    }

    AddCommand(command);
}

void OpenGLCommandBuffer::SetViewport(const glm::uvec2& origin, const glm::uvec2& size) {
    assert(inRenderPass);

    Command command = {};
    command.type = Command::Type::SET_VIEWPORT;

    command.setViewportCommand.origin[0] = origin.x;
    command.setViewportCommand.origin[1] = frameBufferHeight - (origin.y + size.y);
    memcpy(command.setViewportCommand.size, &size[0], sizeof(size));

    AddCommand(command);
}

void OpenGLCommandBuffer::SetScissor(const glm::uvec2& origin, const glm::uvec2& size) {
    assert(inRenderPass);

    Command command = {};
    command.type = Command::Type::SET_SCISSOR;

    command.setScissorCommand.origin[0] = origin.x;
    command.setScissorCommand.origin[1] = frameBufferHeight - (origin.y + size.y);
    memcpy(command.setScissorCommand.size, &size[0], sizeof(size));

    AddCommand(command);
}

void OpenGLCommandBuffer::SetLineWidth(float width) {
    Command command = {};
    command.type = Command::Type::SET_LINE_WIDTH;

    /// @todo Clamp line width to supported values.

    command.setLineWidthCommand.width = width;

    AddCommand(command);
}

void OpenGLCommandBuffer::BindGeometry(GeometryBinding* geometryBinding) {
    assert(inRenderPass);
    assert(geometryBinding != nullptr);

    const OpenGLGeometryBinding* openGLGeometryBinding = static_cast<const OpenGLGeometryBinding*>(geometryBinding);

    indexType = openGLGeometryBinding->GetIndexType();
    switch (indexType) {
    case GL_UNSIGNED_SHORT:
        indexSize = sizeof(GLushort);
        break;
    case GL_UNSIGNED_INT:
        indexSize = sizeof(GLuint);
        break;
    }

    Command command = {};
    command.type = Command::Type::BIND_GEOMETRY;

    command.bindGeometryCommand.vertexArray = openGLGeometryBinding->GetVertexArray();

    AddCommand(command);
}

void OpenGLCommandBuffer::BindUniformBuffer(ShaderProgram::BindingType bindingType, Buffer* uniformBuffer) {
    assert(uniformBuffer != nullptr);
    assert(uniformBuffer->GetBufferUsage() == Buffer::BufferUsage::UNIFORM_BUFFER);

    Command command = {};
    command.type = Command::Type::BIND_UNIFORM_BUFFER;

    switch (bindingType) {
    case ShaderProgram::BindingType::MATRICES:
        command.bindUniformBufferCommand.index = 0;
        break;
    case ShaderProgram::BindingType::UNIFORMS:
        command.bindUniformBufferCommand.index = 1;
        break;
    default:
        assert(false);
    }

    command.bindUniformBufferCommand.uniformBuffer = static_cast<OpenGLBuffer*>(uniformBuffer)->GetBufferID();

    AddCommand(command);
}

void OpenGLCommandBuffer::BindStorageBuffer(Buffer* storageBuffer) {
    assert(storageBuffer != nullptr);
    assert(storageBuffer->GetBufferUsage() == Buffer::BufferUsage::STORAGE_BUFFER || storageBuffer->GetBufferUsage() == Buffer::BufferUsage::VERTEX_STORAGE_BUFFER);

    Command command = {};
    command.type = Command::Type::BIND_STORAGE_BUFFER;

    command.bindStorageBufferCommand.index = 0;
    command.bindStorageBufferCommand.storageBuffer = static_cast<OpenGLBuffer*>(storageBuffer)->GetBufferID();

    AddCommand(command);
}

void OpenGLCommandBuffer::BindMaterial(std::initializer_list<Texture*> textures) {
    Command command = {};
    command.type = Command::Type::BIND_TEXTURE;

    GLenum slot = GL_TEXTURE0;
    for (const Texture* texture : textures) {
        assert(texture != nullptr);

        command.bindTextureCommand.slot = slot++;
        command.bindTextureCommand.texture = static_cast<const OpenGLTexture*>(texture)->GetID();

        AddCommand(command);
    }
}

void OpenGLCommandBuffer::PushConstants(const void* data) {
    assert(data != nullptr);
    assert(currentShaderProgram != nullptr);

    const std::vector<OpenGLShaderProgram::PushConstant>& pushConstants = currentShaderProgram->GetPushConstants();
    assert(pushConstants.size() > 0);

    for (OpenGLShaderProgram::PushConstant pushConstant : pushConstants) {
        void* value = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(data) + pushConstant.offset);

        switch (pushConstant.type) {
        case ShaderSource::ReflectionInfo::PushConstant::Type::VEC3:
            SetUniformVector3(pushConstant.uniformLocation, *reinterpret_cast<const glm::vec3*>(value));
            break;
        case ShaderSource::ReflectionInfo::PushConstant::Type::VEC4:
            SetUniformVector4(pushConstant.uniformLocation, *reinterpret_cast<const glm::vec4*>(value));
            break;
        case ShaderSource::ReflectionInfo::PushConstant::Type::MAT4:
            SetUniformMatrix4(pushConstant.uniformLocation, *reinterpret_cast<const glm::mat4*>(value));
            break;
        }
    }
}

void OpenGLCommandBuffer::Draw(unsigned int vertexCount, unsigned int firstVertex) {
    assert(inRenderPass);

    Command command = {};
    command.type = Command::Type::DRAW;

    command.drawCommand.mode = primitiveType;
    command.drawCommand.first = firstVertex;
    command.drawCommand.count = vertexCount;

    AddCommand(command);
}

void OpenGLCommandBuffer::DrawIndexed(unsigned int indexCount, unsigned int firstIndex, unsigned int baseVertex) {
    assert(inRenderPass);

    Command command = {};
    command.type = Command::Type::DRAW_INDEXED;

    command.drawIndexedCommand.mode = primitiveType;
    command.drawIndexedCommand.count = indexCount;
    command.drawIndexedCommand.type = indexType;
    command.drawIndexedCommand.indices = reinterpret_cast<GLvoid*>(static_cast<uintptr_t>(firstIndex) * indexSize);
    command.drawIndexedCommand.baseVertex = baseVertex;

    AddCommand(command);
}

void OpenGLCommandBuffer::BlitToSwapChain(Texture* texture) {
    assert(!inRenderPass);
    assert(texture != nullptr);

    Command command = {};
    command.type = Command::Type::BLIT_TO_SWAP_CHAIN;

    command.blitToSwapChainCommand.texture = static_cast<OpenGLTexture*>(texture)->GetID();

    AddCommand(command);
}

void OpenGLCommandBuffer::BindComputePipeline(ComputePipeline* computePipeline) {
    assert(!inRenderPass);
    assert(computePipeline != nullptr);

    OpenGLComputePipeline* openGLComputePipeline = static_cast<OpenGLComputePipeline*>(computePipeline);
    currentShaderProgram = openGLComputePipeline->GetShaderProgram();

    Command command = {};
    command.type = Command::Type::BIND_COMPUTE_PIPELINE;

    command.bindComputePipelineCommand.program = currentShaderProgram->GetID();

    AddCommand(command);
}

void OpenGLCommandBuffer::Dispatch(const glm::uvec3& numGroups) {
    assert(!inRenderPass);

    Command command = {};
    command.type = Command::Type::DISPATCH;

    command.dispatchCommand.groupsX = numGroups.x;
    command.dispatchCommand.groupsY = numGroups.y;
    command.dispatchCommand.groupsZ = numGroups.z;

    AddCommand(command);

    // Memory barrier.
    command.type = Command::Type::MEMORY_BARRIER;
    AddCommand(command);
}

void OpenGLCommandBuffer::Submit() {
    for (const Command& command : commands) {
        SubmitCommand(command);
    }

    commands.clear();
    timings.clear();
}

const std::vector<OpenGLCommandBuffer::Timing>& OpenGLCommandBuffer::GetTimings() const {
    return timings;
}

void OpenGLCommandBuffer::SetUniformInteger(unsigned int location, int value) {
    Command command = {};
    command.type = Command::Type::SET_UNIFORM_INTEGER;

    command.setUniformIntegerCommand.location = location;
    command.setUniformIntegerCommand.value = value;

    AddCommand(command);
}

void OpenGLCommandBuffer::SetUniformFloat(unsigned int location, float value) {
    Command command = {};
    command.type = Command::Type::SET_UNIFORM_FLOAT;

    command.setUniformFloatCommand.location = location;
    command.setUniformFloatCommand.value = value;

    AddCommand(command);
}

void OpenGLCommandBuffer::SetUniformVector2(unsigned int location, const glm::vec2& value) {
    Command command = {};
    command.type = Command::Type::SET_UNIFORM_VECTOR2;

    command.setUniformVector2Command.location = location;
    memcpy(command.setUniformVector2Command.value, &value[0], sizeof(value));

    AddCommand(command);
}

void OpenGLCommandBuffer::SetUniformVector3(unsigned int location, const glm::vec3& value) {
    Command command = {};
    command.type = Command::Type::SET_UNIFORM_VECTOR3;

    command.setUniformVector3Command.location = location;
    memcpy(command.setUniformVector3Command.value, &value[0], sizeof(value));

    AddCommand(command);
}

void OpenGLCommandBuffer::SetUniformVector4(unsigned int location, const glm::vec4& value) {
    Command command = {};
    command.type = Command::Type::SET_UNIFORM_VECTOR4;

    command.setUniformVector4Command.location = location;
    memcpy(command.setUniformVector4Command.value, &value[0], sizeof(value));

    AddCommand(command);
}

void OpenGLCommandBuffer::SetUniformMatrix3(unsigned int location, const glm::mat3& value) {
    Command command = {};
    command.type = Command::Type::SET_UNIFORM_MATRIX3;

    command.setUniformMatrix3Command.location = location;
    memcpy(command.setUniformMatrix3Command.value, &value[0][0], sizeof(value));

    AddCommand(command);
}

void OpenGLCommandBuffer::SetUniformMatrix4(unsigned int location, const glm::mat4& value) {
    Command command = {};
    command.type = Command::Type::SET_UNIFORM_MATRIX4;

    command.setUniformMatrix4Command.location = location;
    memcpy(command.setUniformMatrix4Command.value, &value[0][0], sizeof(value));

    AddCommand(command);
}

void OpenGLCommandBuffer::AddCommand(const Command& command) {
    commands.push_back(command);
}

void OpenGLCommandBuffer::SubmitCommand(const Command& command) {
    switch (command.type) {
    case Command::Type::BEGIN_RENDER_PASS: {
        if (openGLRenderer.IsProfiling()) {
            timingIndex = command.beginRenderPassCommand.timingIndex;
            glQueryCounter(timings[timingIndex].startQuery, GL_TIMESTAMP);
        }
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, command.beginRenderPassCommand.frameBuffer);
        if (command.beginRenderPassCommand.clearMask != 0) {
            glDisable(GL_SCISSOR_TEST);
            glDepthMask(GL_TRUE);
            glClear(command.beginRenderPassCommand.clearMask);
            glEnable(GL_SCISSOR_TEST);
        }
        break;
    }
    case Command::Type::END_RENDER_PASS: {
        glBindVertexArray(0);
        if (openGLRenderer.IsProfiling())
            glQueryCounter(timings[timingIndex].endQuery, GL_TIMESTAMP);
        break;
    }
    case Command::Type::BIND_GRAPHICS_PIPELINE: {
        // Shader program.
        glUseProgram(command.bindGraphicsPipelineCommand.program);

        // Polygon mode.
        glPolygonMode(GL_FRONT_AND_BACK, command.bindGraphicsPipelineCommand.polygonMode);

        // Cull face.
        if (command.bindGraphicsPipelineCommand.cullFaceEnabled) {
            glEnable(GL_CULL_FACE);
            glCullFace(command.bindGraphicsPipelineCommand.cullFace);
        } else {
            glDisable(GL_CULL_FACE);
        }

        // Depth test.
        if (command.bindGraphicsPipelineCommand.depthTestEnabled) {
            glEnable(GL_DEPTH_TEST);
            glDepthMask(command.bindGraphicsPipelineCommand.depthMaskEnabled);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        // Depth function.
        glDepthFunc(command.bindGraphicsPipelineCommand.depthFunction);

        // Blending.
        if (command.bindGraphicsPipelineCommand.blendingEnabled) {
            glEnable(GL_BLEND);
            glBlendFunc(command.bindGraphicsPipelineCommand.blendingSourceFactor, command.bindGraphicsPipelineCommand.blendingDestinationFactor);
        } else {
            glDisable(GL_BLEND);
        }
        break;
    }
    case Command::Type::SET_VIEWPORT: {
        glViewport(command.setViewportCommand.origin[0], command.setViewportCommand.origin[1], command.setViewportCommand.size[0], command.setViewportCommand.size[1]);
        break;
    }
    case Command::Type::SET_SCISSOR: {
        glScissor(command.setScissorCommand.origin[0], command.setScissorCommand.origin[1], command.setScissorCommand.size[0], command.setScissorCommand.size[1]);
        break;
    }
    case Command::Type::SET_LINE_WIDTH: {
        glLineWidth(command.setLineWidthCommand.width);
        break;
    }
    case Command::Type::BIND_GEOMETRY: {
        glBindVertexArray(command.bindGeometryCommand.vertexArray);
        break;
    }
    case Command::Type::SET_UNIFORM_INTEGER: {
        glUniform1i(command.setUniformIntegerCommand.location, command.setUniformIntegerCommand.value);
        break;
    }
    case Command::Type::SET_UNIFORM_FLOAT: {
        glUniform1f(command.setUniformFloatCommand.location, command.setUniformFloatCommand.value);
        break;
    }
    case Command::Type::SET_UNIFORM_VECTOR2: {
        glUniform2fv(command.setUniformVector2Command.location, 1, command.setUniformVector2Command.value);
        break;
    }
    case Command::Type::SET_UNIFORM_VECTOR3: {
        glUniform3fv(command.setUniformVector3Command.location, 1, command.setUniformVector3Command.value);
        break;
    }
    case Command::Type::SET_UNIFORM_VECTOR4: {
        glUniform4fv(command.setUniformVector4Command.location, 1, command.setUniformVector4Command.value);
        break;
    }
    case Command::Type::SET_UNIFORM_MATRIX3: {
        glUniformMatrix3fv(command.setUniformMatrix3Command.location, 1, GL_FALSE, command.setUniformMatrix3Command.value);
        break;
    }
    case Command::Type::SET_UNIFORM_MATRIX4: {
        glUniformMatrix4fv(command.setUniformMatrix4Command.location, 1, GL_FALSE, command.setUniformMatrix4Command.value);
        break;
    }
    case Command::Type::BIND_TEXTURE: {
        glActiveTexture(command.bindTextureCommand.slot);
        glBindTexture(GL_TEXTURE_2D, command.bindTextureCommand.texture);
        break;
    }
    case Command::Type::BIND_UNIFORM_BUFFER: {
        glBindBufferBase(GL_UNIFORM_BUFFER, command.bindUniformBufferCommand.index, command.bindUniformBufferCommand.uniformBuffer);
        break;
    }
    case Command::Type::BIND_STORAGE_BUFFER: {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, command.bindStorageBufferCommand.index, command.bindStorageBufferCommand.storageBuffer);
        break;
    }
    case Command::Type::DRAW: {
        glDrawArrays(command.drawCommand.mode, command.drawCommand.first, command.drawCommand.count);
        break;
    }
    case Command::Type::DRAW_INDEXED: {
        glDrawElementsBaseVertex(command.drawIndexedCommand.mode, command.drawIndexedCommand.count, command.drawIndexedCommand.type, command.drawIndexedCommand.indices, command.drawIndexedCommand.baseVertex);
        break;
    }
    case Command::Type::BLIT_TO_SWAP_CHAIN: {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_BLEND);
        glUseProgram(blitShaderProgram->GetID());
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, command.blitToSwapChainCommand.texture);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glEnable(GL_SCISSOR_TEST);
        break;
    }
    case Command::Type::BIND_COMPUTE_PIPELINE: {
        glUseProgram(command.bindComputePipelineCommand.program);
        break;
    }
    case Command::Type::DISPATCH: {
        glDispatchCompute(command.dispatchCommand.groupsX, command.dispatchCommand.groupsY, command.dispatchCommand.groupsZ);
        break;
    }
    case Command::Type::MEMORY_BARRIER: {
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
        break;
    }
    }
}

} // namespace Video
