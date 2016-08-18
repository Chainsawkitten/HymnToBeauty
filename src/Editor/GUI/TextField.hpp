#pragma once

#include "Widget.hpp"
#include <string>

class Font;
namespace Geometry {
    class Rectangle;
}

namespace GUI {
    /// A field where the user can input text.
    class TextField : public Widget {
        public:
            /// Create new text field.
            /**
             * @param parent Parent widget.
             * @param font %Font to display text with.
             */
            TextField(Widget* parent, Font* font);
            
            /// Destructor.
            ~TextField() override;
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            void Render() override;
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 GetSize() const override;
            
            /// Set the size of the widget.
            /**
             * @param size The new size.
             */
            void SetSize(const glm::vec2& size) override;
            
            /// Get the text field's text.
            /**
             * @return The text in the text field.
             */
            std::string GetText() const;
            
            /// Set the text field's text.
            /**
             * @param text The text to set.
             */
            void SetText(const std::string& text);
            
        protected:
            /// Called when text has been updated.
            virtual void TextUpdated();
            
        private:
            std::string text = "";
            std::size_t markerPosition = 0U;
            bool focus = false;
            
            glm::vec2 size;
            
            Geometry::Rectangle* rectangle;
            Font* font;
    };
}
