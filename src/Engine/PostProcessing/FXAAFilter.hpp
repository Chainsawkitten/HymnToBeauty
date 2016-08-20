#pragma once

#include "Filter.hpp"
#include <glm/glm.hpp>
#include <GL/glew.h>

/// Fast approximate anti-aliasing filter.
class FXAAFilter : public Filter {
    public:
        /// Create new FXAA filter.
        FXAAFilter();
        
        /// Free allocated resources.
        ~FXAAFilter();
        
        /// Get shader program.
        /**
         * @return Shader program
         */
        ShaderProgram* GetShaderProgram() const;
        
        /// Set uniforms.
        void SetUniforms();
        
        /// Set the screen size used when calculating FXAA.
        /**
         * @param screenSize Size of the screen in pixels.
         */
        void SetScreenSize(const glm::vec2& screenSize);
        
        /// Set brightness.
        /**
         * @param brightness Brightness.
         */
        void SetBrightness(float brightness);
        
    private:
        ShaderProgram* shaderProgram;
        
        glm::vec2 screenSize;
        GLint screenSizeLocation;
        
        float brightness = 2.2f;
        GLint brightnessLocation;
};
