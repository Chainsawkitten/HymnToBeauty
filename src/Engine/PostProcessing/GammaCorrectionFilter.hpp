#pragma once

#include "Filter.hpp"
#include <GL/glew.h>

class Shader;

/// Gamma correction filter.
class GammaCorrectionFilter : public Filter {
    public:
        /// Create new color filter.
        GammaCorrectionFilter();
        
        /// Free allocated resources.
        ~GammaCorrectionFilter();
        
        /// Get shader program.
        /**
         * @return Shader program
         */
        ShaderProgram* GetShaderProgram() const;
        
        /// Set uniforms.
        void SetUniforms();
        
        /// Set brightness.
        /**
         * @param brightness Brightness.
         */
        void SetBrightness(float brightness);
        
    private:
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
        
        float brightness = 2.2f;
        GLint brightnessLocation;
};
