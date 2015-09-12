#ifndef IMAGETEXTBUTTON_HPP
#define IMAGETEXTBUTTON_HPP

#include "Button.hpp"
#include "../Texture/Texture2D.hpp"
#include "Font.hpp"

/** @ingroup Core
 * @{
 */

namespace GUI {
    /// Button displaying both an image and text.
    class ImageTextButton : public Button {
        public:
            /// Create new button.
            /**
             * @param parent Parent widget.
             * @param texture %Button texture.
             * @param font %Font to display text with.
             * @param text Text to display.
             */
            ImageTextButton(Widget* parent, Texture2D* texture, Font* font, const std::string& text);
            
            /// Destructor.
            virtual ~ImageTextButton();
            
            /// Render the widget.
            /**
             * @param screenWidth Width of the screen in pixels.
             * @param screenHeight Height of the screen in pixels.
             */
            void Render(int screenWidth, int screenHeight);
            
            /// Get the image size.
            /**
             * @return How large the displayed image should be.
             */
            glm::vec2 ImageSize() const;
            
            /// Set the image size.
            /**
             * @param size How large the displayed image should be.
             */
            void SetImageSize(const glm::vec2& size);
            
        private:
            Geometry::Rectangle* rectangle;
            
            glm::vec2 imageSize;
            
            // Shaders
            Shader* vertexShader;
            Shader* colorFragmentShader;
            Shader* textureFragmentShader;
            ShaderProgram* colorShaderProgram;
            ShaderProgram* textureShaderProgram;
            
            Texture2D* texture;
            Font* font;
            std::string text;
    };
}

/** @} */

#endif
