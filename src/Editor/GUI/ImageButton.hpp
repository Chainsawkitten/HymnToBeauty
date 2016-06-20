#pragma once

#include "Button.hpp"

class Texture2D;

namespace GUI {
    /// %Button displaying an image.
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
             * @param screenSize Size of the screen in pixels.
             */
            void Render(const glm::vec2& screenSize);
            
        private:
            Geometry::Rectangle* rectangle;
            Texture2D* texture;
    };
}
