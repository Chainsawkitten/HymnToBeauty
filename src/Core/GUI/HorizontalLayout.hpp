#ifndef HORIZONTALLAYOUT_HPP
#define HORIZONTALLAYOUT_HPP

#include "../Geometry/Rectangle.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "Container.hpp"

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
            
            /// Add a widget to the container.
            /**
             * @param widget Widget to add to the container.
             */
            void AddWidget(Widget* widget);
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 Size() const;
            
            /// Set the size of the widget.
            /**
             * @param size The new size.
             */
            void SetSize(const glm::vec2& size);
            
        private:
            Geometry::Rectangle* rectangle;
            
            // Shaders
            Shader* vertexShader;
            Shader* fragmentShader;
            ShaderProgram* shaderProgram;
            
            glm::vec2 size;
            
            glm::vec2 nextPosition;
    };
}

/** @} */

#endif
