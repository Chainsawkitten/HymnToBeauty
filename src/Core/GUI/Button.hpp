#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../Geometry/Rectangle.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Texture/Texture2D.hpp"
#include "Widget.hpp"

/** @ingroup Core
 * @{
 */

namespace GUI {
    class Button : public Widget {
        public:
            /// Create new button.
            /**
             * @param parent Parent widget.
             */
            Button(Widget* parent);
            
            /// Destructor.
            virtual ~Button();
            
            /// Update the widget.
            /**
             * @param window Window to get input for.
             */
            void Update(GLFWwindow* window);
            
            /// Render the widget.
            /**
             * @param screenWidth Width of the screen in pixels.
             * @param screenHeight Height of the screen in pixels.
             */
            void Render(int screenWidth, int screenHeight);
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 Size() const;
            
        private:
            Geometry::Rectangle* rectangle;
            
            // Shaders
            Shader* vertexShader;
            Shader* colorFragmentShader;
            Shader* textureFragmentShader;
            ShaderProgram* colorShaderProgram;
            ShaderProgram* textureShaderProgram;
            
            glm::vec2 size;
            
            bool mouseHover;
            
            Texture2D* texture;
    };
}

/** @} */

#endif
