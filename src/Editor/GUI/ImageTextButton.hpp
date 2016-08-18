#pragma once

#include "Button.hpp"

class Texture2D;
class Font;

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
            ~ImageTextButton() override;
            
            /// Render the widget.
            void Render() override;
            
            /// Get the image size.
            /**
             * @return How large the displayed image should be.
             */
            glm::vec2 GetImageSize() const;
            
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
