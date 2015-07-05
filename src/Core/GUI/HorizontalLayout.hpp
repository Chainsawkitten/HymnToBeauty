#ifndef HORIZONTALLAYOUT_HPP
#define HORIZONTALLAYOUT_HPP

#include "Container.hpp"
#include "../Geometry/Rectangle.hpp"
#include "../Shader/ShaderProgram.hpp"

/** @ingroup Core
 * @{
 */

namespace GUI {
    /// Horizontal container.
    class HorizontalLayout : public Container {
        public:
            /// Create new horizontal layout.
            /**
             * @param parent Parent widget.
             */
            HorizontalLayout(Widget* parent);
            
            /// Destructor.
            ~HorizontalLayout();
            
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
