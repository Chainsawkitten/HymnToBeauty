#pragma once

#include "Filter.hpp"
#include <glm/glm.hpp>

namespace Video {
    /// Fast approximate anti-aliasing filter.
    class FXAAFilter : public Filter {
        public:
            /// Create new FXAA filter.
            FXAAFilter();

            /// Free allocated resources.
            ~FXAAFilter();

            Video::ShaderProgram* GetShaderProgram() const final;
            GLuint GetDiffuseLocation() const final;
            void SetUniforms() final;

            /// Set the screen size used when calculating FXAA.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void SetScreenSize(const glm::vec2& screenSize);

        private:
            ShaderProgram* shaderProgram;

            glm::vec2 screenSize;
            GLuint screenSizeLocation;
            GLuint diffuseLocation;
    };
}
