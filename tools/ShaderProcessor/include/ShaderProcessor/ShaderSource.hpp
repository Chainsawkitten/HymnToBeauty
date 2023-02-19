#pragma once

#include <cstdint>
#include <string>
#include <cassert>

/// Contains source code and reflection info for a shader.
struct ShaderSource {
    /// The length of the GLSL source.
    unsigned int glslLength;
    
    /// The GLSL source code.
    const char* glslSource;
    
    /// The length of the Vulkan SPIR-V source.
    unsigned int vulkanSpirvLength;
    
    /// The Vulkan SPIR-V source code.
    const uint32_t* vulkanSpirvSource;

    /// The length of the WebGPU SPIR-V source.
    unsigned int webGPUSpirvLength;

    /// The WebGPU SPIR-V source code.
    const uint32_t* webGPUSpirvSource;
    
    /// Information about the shader.
    struct ReflectionInfo {
        /// Information about a push constant used in the shader.
        struct PushConstant {
            /// The GLSL data type.
            enum class Type {
                INT, ///< Scalar int.
                UINT, ///< Scalar unsigned int.
                FLOAT, ///< Scalar float.
                VEC2, ///< 2 component vector.
                IVEC2, ///< 2 component signed int vector.
                UVEC2, ///< 2 component unsigned int vector.
                VEC3, ///< 3 component vector.
                IVEC3, ///< 3 component signed int vector.
                UVEC3, ///< 3 component unsigned int vector.
                VEC4, ///< 4 component vector.
                IVEC4, ///< 4 component signed int vector.
                UVEC4, ///< 4 component unsigned int vector.
                MAT4 ///< 4x4 matrix.
            };

            /// The type of the push constant.
            Type type;

            /// The name of the push constant.
            std::string name;
        };

        /// Information about a storage buffer used in the shader.
        struct StorageBuffer {
            /// The binding within the descriptor set.
            uint32_t binding;

            /// Whether the storage buffer is read/write (otherwise read only).
            bool readWrite;
        };

        /// Whether a matrix uniform buffer is used.
        bool hasMatrices;

        /// Whether a uniform buffer is used.
        bool hasUniforms;

        /// The number of storage buffers used.
        unsigned int storageBufferCount;

        /// The storage buffers.
        StorageBuffer* storageBuffers;
        
        /// The number of samplers used.
        unsigned int materialCount;

        /// The number of push constants used.
        unsigned int pushConstantCount;

        /// The push constants.
        PushConstant* pushConstants;

        /// The name of the push constant uniform block (for OpenGL).
        std::string pushConstantBlockName;
    };
    
    /// Reflection info.
    ReflectionInfo reflectionInfo;
};

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
    case ShaderSource::ReflectionInfo::PushConstant::Type::IVEC2:
    case ShaderSource::ReflectionInfo::PushConstant::Type::UVEC2:
        return sizeof(float) * 2;
    case ShaderSource::ReflectionInfo::PushConstant::Type::VEC3:
    case ShaderSource::ReflectionInfo::PushConstant::Type::IVEC3:
    case ShaderSource::ReflectionInfo::PushConstant::Type::UVEC3:
        return sizeof(float) * 3;
    case ShaderSource::ReflectionInfo::PushConstant::Type::VEC4:
    case ShaderSource::ReflectionInfo::PushConstant::Type::IVEC4:
    case ShaderSource::ReflectionInfo::PushConstant::Type::UVEC4:
        return sizeof(float) * 4;
    case ShaderSource::ReflectionInfo::PushConstant::Type::MAT4:
        return sizeof(float) * 4 * 4;
    default:
        assert(false);
        return 0;
    }
}
