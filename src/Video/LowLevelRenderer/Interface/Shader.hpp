#pragma once

namespace Video {

class Buffer;

/// Compiles and handles a shader. Shaders should be linked together into a ShaderProgram.
class Shader {
  public:
    /// The type of shader.
    enum class Type {
        VERTEX_SHADER, ///< Vertex shader.
        FRAGMENT_SHADER, ///< Fragment shader.
        COMPUTE_SHADER, ///< Compute shader.
    };

    /// Create a new shader.
    Shader() {}

    /// Destructor.
    virtual ~Shader() {}

  private:
    Shader(const Shader& other) = delete;
};

}
