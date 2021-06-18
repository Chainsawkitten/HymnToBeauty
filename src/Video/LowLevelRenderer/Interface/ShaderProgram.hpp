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

  protected:
    /// Get the size of a push constant type.
    /**
     * @param type The type of the push constant.
     *
     * @return The size of the type in bytes.
     */
    static uint32_t PushConstantTypeToSize(ShaderSource::ReflectionInfo::PushConstant::Type type) {
        switch (type) {
        case ShaderSource::ReflectionInfo::PushConstant::Type::INT:
            return sizeof(int32_t);
        case ShaderSource::ReflectionInfo::PushConstant::Type::UINT:
            return sizeof(uint32_t);
        case ShaderSource::ReflectionInfo::PushConstant::Type::FLOAT:
            return sizeof(float);
        case ShaderSource::ReflectionInfo::PushConstant::Type::VEC2:
            return sizeof(glm::vec2);
        case ShaderSource::ReflectionInfo::PushConstant::Type::VEC3:
            return sizeof(glm::vec3);
        case ShaderSource::ReflectionInfo::PushConstant::Type::VEC4:
            return sizeof(glm::vec4);
        case ShaderSource::ReflectionInfo::PushConstant::Type::MAT4:
            return sizeof(glm::mat4);
        default:
            assert(false);
            return 0;
        }
    }

  private:
    ShaderProgram(const ShaderProgram& other) = delete;
};

}
