#pragma once

#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Shader/ShaderProgram.hpp>
#include "Widget.hpp"
#include <functional>

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
            ~Button() override;
            
            /// Update the widget.
            void Update() override;
            
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
            
            /// Set function to call when clicked.
            /**
             * @param callback Function to call when button is clicked.
             */
            void SetClickedCallback(std::function<void()> callback);
            
            /// Get whether the mouse is over the button.
            /**
             * @return Whether the mouse is over the button
             */
            bool GetMouseHover() const;
            
        private:
            Geometry::Rectangle* rectangle;
            
            glm::vec2 size;
            
            // Interaction
            bool mouseHover;
            bool hasClickedCallback;
            std::function<void()> clickedCallback;
    };
}
