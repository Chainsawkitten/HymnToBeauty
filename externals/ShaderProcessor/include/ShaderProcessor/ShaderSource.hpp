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
                VEC3, ///< 3 component vector.
                VEC4, ///< 4 component vector.
                MAT4 ///< 4x4 matrix.
            };

            /// The type of the push constant.
            Type type;

            /// The name of the push constant.
            std::string name;
        };

        /// Whether a matrix uniform buffer is used.
        bool hasMatrices;

        /// Whether a uniform buffer is used.
        bool hasUniforms;

        /// Whether a storage buffer is used.
        bool hasStorageBuffer;

        /// Whether the storage buffer is read/write (otherwise read only).
        bool storageBufferReadWrite;
        
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
