#pragma once

#include "../Interface/ComputePipeline.hpp"

namespace Video {

class ShaderProgram;
class OpenGLShaderProgram;

/// OpenGL implementation of ComputePipeline.
class OpenGLComputePipeline : public ComputePipeline {
  public:
    /// Create new OpenGL compute pipeline.
    /**
     * @param shaderProgram The shader program to use.
     */
    OpenGLComputePipeline(const ShaderProgram* shaderProgram);

    /// Destructor.
    ~OpenGLComputePipeline() final;

    /// Get the shader program.
    /**
     * @return The shader program.
     */
    const OpenGLShaderProgram* GetShaderProgram() const;

  private:
    OpenGLComputePipeline(const OpenGLComputePipeline& other) = delete;

    const OpenGLShaderProgram* shaderProgram;
};

}
