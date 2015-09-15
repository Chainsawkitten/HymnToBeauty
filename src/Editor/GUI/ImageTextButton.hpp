#ifndef IMAGETEXTBUTTON_HPP
#define IMAGETEXTBUTTON_HPP

#include "Button.hpp"
#include <Core/Texture/Texture2D.hpp>
#include <Core/Font/Font.hpp>

/** @ingroup Editor
 * @{
 */

namespace GUI {
    /// %Button displaying both an image and text.
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
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize);
            
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
            
            Texture2D* texture;
            Font* font;
            std::string text;
    };
}

/** @} */

#endif
