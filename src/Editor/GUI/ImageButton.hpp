#ifndef IMAGEBUTTON_HPP
#define IMAGEBUTTON_HPP

#include "Button.hpp"
#include <Core/Texture/Texture2D.hpp>

/** @ingroup Core
 * @{
 */

namespace GUI {
    /// Button displaying an image.
    class ImageButton : public Button {
        public:
            /// Create new button.
            /**
             * @param parent Parent widget.
             * @param texture %Button texture.
             */
            ImageButton(Widget* parent, Texture2D* texture);
            
            /// Destructor.
            virtual ~ImageButton();
            
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
            Shader* colorFragmentShader;
            Shader* textureFragmentShader;
            ShaderProgram* colorShaderProgram;
            ShaderProgram* textureShaderProgram;
            
            Texture2D* texture;
    };
}

/** @} */

#endif
