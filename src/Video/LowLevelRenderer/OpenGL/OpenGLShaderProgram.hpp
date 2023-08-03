#pragma once

#include "../Interface/ShaderProgram.hpp"

#include <glad/gl.h>
#include <initializer_list>
#include <vector>

namespace Video {

class Shader;

/// OpenGL implementation of ShaderProgram.
class OpenGLShaderProgram : public ShaderProgram {
  public:
    /// Information about a push constant.
    struct PushConstant {
        /// The uniform location to set.
        GLint uniformLocation;

        /// The data type of the push constant.
        ShaderSource::ReflectionInfo::PushConstant::Type type;

        /// The offset into the push constant structure.
        uint32_t offset;
    };

    /// Create new OpenGL shader program.
    /**
     * @param shaders List of shaders to link together.
     */
    explicit OpenGLShaderProgram(std::initializer_list<const Shader*> shaders);

    /// Destructor.
    ~OpenGLShaderProgram() final;

    /// Get shader program ID.
    /**
     * @return The ID used in OpenGL calls.
     */
    unsigned int GetID() const;

    /// Get information about the push constants in this shader program.
    /**
     * Push constants are implemented using uniforms.
     * @return A list of the push constants used in the shader program.
     */
    const std::vector<PushConstant>& GetPushConstants() const;

    /// Get whether the shader program writes to a storage buffer.
    /**
     * @return Whether the shader program writes to a storage buffer.
     */
    bool WritesToStorageBuffer() const;

  private:
    OpenGLShaderProgram(const OpenGLShaderProgram& other) = delete;

    void AddPushConstants(const ShaderSource::ReflectionInfo& reflectionInfo);

    GLuint shaderProgram;
    std::vector<PushConstant> pushConstants;

    bool writesToStorageBuffer = false;
};

}
