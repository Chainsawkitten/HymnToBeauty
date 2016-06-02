#pragma once

#include "Button.hpp"
#include <Engine/Font/Font.hpp>

namespace GUI {
    /// %Button displaying text.
    class TextButton : public Button {
        public:
            /// Create new button.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             * @param text Text to display.
             */
            TextButton(Widget* parent, Font* font, const std::string& text);
            
            /// Destructor.
            ~TextButton() override;
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize) override;
            
        private:
            Geometry::Rectangle* rectangle;
            
            Font* font;
            std::string text;
    };
}
