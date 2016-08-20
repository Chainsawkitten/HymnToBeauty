#pragma once

#include "Filter.hpp"
#include <glm/glm.hpp>
#include <GL/glew.h>

class Shader;

/// Color adjustment filter.
class ColorFilter : public Filter {
    public:
        /// Create new color filter.
        /**
         * @param color Color.
         */
        ColorFilter(const glm::vec3& color);
        
        /// Free allocated resources.
        ~ColorFilter();
        
        /// Set color.
        /**
         * @param color Color.
         */
        void SetColor(const glm::vec3& color);
        
        /// Get shader program.
        /**
         * @return Shader program
         */
        ShaderProgram* GetShaderProgram() const;
        
        /// Set uniforms.
        void SetUniforms();
        
    private:
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
        
        glm::vec3 color;
        GLint colorLocation;
};
