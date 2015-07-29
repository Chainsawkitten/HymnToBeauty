#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "../Geometry/Rectangle.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Texture/Texture2D.hpp"
#include "Widget.hpp"
#include <functional>

/** @ingroup Core
 * @{
 */

namespace GUI {
    class Button : public Widget {
        public:
            /// Create new button.
            /**
             * @param parent Parent widget.
             * @param texture %Button texture.
             */
            Button(Widget* parent, Texture2D* texture);
            
            /// Destructor.
            virtual ~Button();
            
            /// Update the widget.
            void Update();
            
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
            
            /// Set function to call when clicked.
            /**
             * @param callback Function to call when button is clicked.
             */
            void SetClickedCallback(std::function<void()> callback);
            
        private:
            Geometry::Rectangle* rectangle;
            
            // Shaders
            Shader* vertexShader;
            Shader* colorFragmentShader;
            Shader* textureFragmentShader;
            ShaderProgram* colorShaderProgram;
            ShaderProgram* textureShaderProgram;
            
            glm::vec2 size;
            
            // Interaction
            bool mouseHover;
            bool hasClickedCallback;
            std::function<void()> clickedCallback;
            
            Texture2D* texture;
    };
}

/** @} */

#endif
