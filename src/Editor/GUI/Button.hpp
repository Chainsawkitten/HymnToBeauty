#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Shader/ShaderProgram.hpp>
#include "Widget.hpp"
#include <functional>

/** @ingroup Editor
 * @{
 */

namespace GUI {
    /// %Button widget.
    class Button : public Widget {
        public:
            /// Create new button.
            /**
             * @param parent Parent widget.
             */
            Button(Widget* parent);
            
            /// Destructor.
            virtual ~Button();
            
            /// Update the widget.
            virtual void Update();
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            virtual void Render(const glm::vec2& screenSize) = 0;
            
            /// Get the size of the widget.
            /**
             * @return The size
             */
            glm::vec2 Size() const;
            
            /// Set the size of the widget.
            /**
             * @param size New widget size.
             */
            void SetSize(const glm::vec2& size);
            
            /// Set function to call when clicked.
            /**
             * @param callback Function to call when button is clicked.
             */
            void SetClickedCallback(std::function<void()> callback);
            
            /// Get whether the mouse is over the button.
            /**
             * @return Whether the mouse is over the button
             */
            bool MouseHover() const;
            
        private:
            Geometry::Rectangle* rectangle;
            
            glm::vec2 size;
            
            // Interaction
            bool mouseHover;
            bool hasClickedCallback;
            std::function<void()> clickedCallback;
    };
}

/** @} */

#endif
