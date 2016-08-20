#pragma once

class ShaderProgram;

/// Post-processing filter.
class Filter {
    public:
        /// Create filter.
        Filter() { }
        
        /// Free allocated resources.
        virtual ~Filter() { }
        
        /// Get shader program.
        /**
         * @return Shader program
         */
        virtual ShaderProgram* GetShaderProgram() const = 0;
        
        /// Set uniforms.
        virtual void SetUniforms() = 0;
};
