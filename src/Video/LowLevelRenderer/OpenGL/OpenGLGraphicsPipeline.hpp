#pragma once

#include "../Interface/GraphicsPipeline.hpp"

#include <glad/gl.h>

namespace Video {

class ShaderProgram;
class OpenGLShaderProgram;

/// OpenGL implementation of GraphicsPipeline.
class OpenGLGraphicsPipeline : public GraphicsPipeline {
  public:
    /// Create new OpenGL graphics pipeline.
    /**
     * @param shaderProgram The shader program to use.
     * @param configuration The configuration of the pipeline.
     */
    OpenGLGraphicsPipeline(const ShaderProgram* shaderProgram, const Configuration& configuration);

    /// Destructor.
    ~OpenGLGraphicsPipeline() final;

    /// Get the shader program.
    /**
     * @return The shader program.
     */
    const OpenGLShaderProgram* GetShaderProgram() const;

    /// Get the pipeline configuration.
    /**
     * @return The pipeline configuration
     */
    const Configuration& GetConfiguration() const;

  private:
    OpenGLGraphicsPipeline(const OpenGLGraphicsPipeline& other) = delete;

    const OpenGLShaderProgram* shaderProgram;
    Configuration configuration;
};

}
