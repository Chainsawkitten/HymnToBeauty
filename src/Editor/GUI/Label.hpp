#pragma once

#include "Widget.hpp"
#include <Engine/Font/Font.hpp>

namespace GUI {
    /// Label that displays a text message.
    class Label : public Widget {
        public:
            /// Create new label.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             * @param text Text to display.
             */
            Label(Widget* parent, Font* font, const std::string& text);
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize) override;
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 GetSize() const override;
            
            /// Set the size of the widget.
            /**
             * @param size New widget size.
             */
            void SetSize(const glm::vec2& size) override;
            
            /// Set the text to display.
            /**
             * @param text The text to display.
             */
            void SetText(const std::string& text);
            
        private:
            std::string text;
            
            Font* font;
    };
}
