#pragma once

#include <cstdint>
#include <string>

/// Contains source code and reflection info for a shader.
struct ShaderSource {
    /// The length of the GLSL source.
    unsigned int glslLength;
    
    /// The GLSL source code.
    const char* glslSource;
    
    /// The length of the SPIR-V source.
    unsigned int spirvLength;
    
    /// The SPIR-V source code.
    const uint32_t* spirvSource;
    
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
                VEC3, ///< 3 component vector.
                VEC4, ///< 4 component vector.
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
