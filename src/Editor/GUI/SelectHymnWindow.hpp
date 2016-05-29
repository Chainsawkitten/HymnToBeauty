#pragma once

#include "Container.hpp"
#include "ImageButton.hpp"
#include "TextField.hpp"
#include <Engine/Geometry/Rectangle.hpp>

namespace GUI {
    /// A window where a hymn can be selected.
    class SelectHymnWindow : public Container {
        public:
            /// Create new window.
            /**
             * @param parent Parent widget.
             */
            SelectHymnWindow(Widget* parent);
            
            /// Destructor.
            virtual ~SelectHymnWindow();
            
            /// Update the widget.
            void Update();
            
            /// Render the widget.
            /**
             * @param screenSize Size of the screen in pixels.
             */
            virtual void Render(const glm::vec2& screenSize);
            
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
            
            /// Set function to call when closed.
            /**
             * @param callback Function to call when window is closed.
             */
            void SetClosedCallback(std::function<void()> callback);
            
            /// Close the window.
            void Close();
            
        private:
            Geometry::Rectangle* rectangle;
            Font* font;
            
            glm::vec2 size;
            
            // Interaction
            bool hasClosedCallback;
            std::function<void()> closedCallback;
            bool shouldClose;
            
            ImageButton* closeButton;
            Texture2D* closeTexture;
            
            TextField* nameTextField;
    };
}
