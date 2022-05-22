#pragma once

#include <ShaderProcessor/ShaderSource.hpp>
#include <glm/glm.hpp>

namespace Video {

class Buffer;

/// A shader program.
class ShaderProgram {
  public:
    /// A type of bound resource.
    enum BindingType {
        MATRICES, ///< Uniform buffer with matrices for vertex shader.
        MATERIAL, ///< Samplers used in fragment shader.
        UNIFORMS, ///< Uniform buffer.
        STORAGE_BUFFER, ///< Storage buffer.
        BINDING_TYPES ///< Total number of binding types.
    };

    /// Create a new shader program.
    ShaderProgram() {}

    /// Destructor.
    virtual ~ShaderProgram() {}

  private:
    ShaderProgram(const ShaderProgram& other) = delete;
};

}
