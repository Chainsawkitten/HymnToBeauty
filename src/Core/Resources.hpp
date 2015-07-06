#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include "Shader/Shader.hpp"
#include <map>

/** @ingroup Core
 * @{
 */

class Resources {
    public:
        /// Get the instance of the class.
        /**
         * @return The %Resources instance
         */
        static Resources& GetInstance();
        
        /// Create a shader if it doesn't already exist.
        /**
		 * @param source GLSL code for the shader.
		 * @param sourceLength Length of the GLSL source code.
		 * @param shaderType %Shader type. One of GL_COMPUTE_SHADER, GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, or GL_FRAGMENT_SHADER.
		 * @return The shader instance
		 */
        Shader* CreateShader(const char* source, int sourceLength, GLenum shaderType);
        
        /// Free the reference to the shader.
        /**
         * Deletes the instance if no more references exist.
         * @param source GLSL code for the shader.
         */
        void FreeShader(const char* source);
        
    private:
        Resources();
        
        // Shaders
        struct ShaderInstance {
            Shader* shader;
            int count;
        };
        std::map<const char*, ShaderInstance> shaders;
};

/** @} */

#endif
