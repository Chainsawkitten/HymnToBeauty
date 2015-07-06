#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "Widget.hpp"
#include "../Geometry/Rectangle.hpp"
#include "../Shader/ShaderProgram.hpp"

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
            
            /// Render the widget.
            /**
             * @param screenWidth Width of the screen in pixels.
             * @param screenHeight Height of the screen in pixels.
             */
            void Render(int screenWidth, int screenHeight);
            
        private:
            Geometry::Rectangle* rectangle;
            
            // Shaders
            Shader* vertexShader;
            Shader* fragmentShader;
            ShaderProgram* shaderProgram;
    };
}

/** @} */

#endif
