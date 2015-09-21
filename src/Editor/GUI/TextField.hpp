#ifndef TEXTFIELD_HPP
#define TEXTFIELD_HPP

#include "Widget.hpp"
#include <Core/Geometry/Rectangle.hpp>
#include <string>
#include <Core/Font/Font.hpp>

/** @ingroup Editor
 * @{
 */

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
            ~TextField();
            
            /// Update the widget.
            void Update();
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize);
            
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
            
            /// Get the text field's text.
            /**
             * @return The text in the text field.
             */
            std::string Text() const;
            
            /// Set the text field's text.
            /**
             * @param text The text to set.
             */
            void SetText(const std::string& text);
            
        private:
            std::string text;
            
            glm::vec2 size;
            
            Geometry::Rectangle* rectangle;
            Font* font;
    };
}

/** @} */

#endif
