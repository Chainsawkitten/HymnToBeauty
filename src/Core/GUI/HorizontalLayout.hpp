#ifndef HORIZONTALLAYOUT_HPP
#define HORIZONTALLAYOUT_HPP

#include "../Geometry/Rectangle.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "Container.hpp"
#include "Button.hpp"

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
            
            /// Get the size of the horizontal layout.
            /**
             * @return The size
             */
            const glm::vec2& Size() const;
            
            /// Set the size of the horizontal layout.
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
            
            Button* testButton;
    };
}

/** @} */

#endif
