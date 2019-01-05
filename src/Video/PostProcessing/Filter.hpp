#pragma once

#include <glad/glad.h>

namespace Video {
class ShaderProgram;

/// Post-processing filter.
class Filter {
  public:
    /// Create filter.
    Filter() {}

    /// Free allocated resources.
    virtual ~Filter() {}

    /// Get shader program.
    /**
     * @return Shader program
     */
    virtual ShaderProgram* GetShaderProgram() const = 0;

    /// Get the location of the diffuse uniform.
    /**
     * @return The location of the diffuse uniform.
     */
    virtual GLuint GetDiffuseLocation() const = 0;

    /// Set uniforms.
    virtual void SetUniforms() = 0;
};
} // namespace Video
