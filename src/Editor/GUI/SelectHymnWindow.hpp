#pragma once

#include "Container.hpp"
#include "ImageButton.hpp"
#include "TextButton.hpp"
#include "TextField.hpp"
#include "VerticalLayout.hpp"
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
            ~SelectHymnWindow() override;
            
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
             * @param size The new size.
             */
            void SetSize(const glm::vec2& size);
            
            /// Set function to call when closed.
            /**
             * @param callback Function to call when window is closed.
             */
            void SetClosedCallback(std::function<void(const std::string&)> callback);
            
        private:
            void Close();
            void Select();
            void SetHymn(const std::string& name);
            
            Geometry::Rectangle* rectangle;
            Font* font;
            
            glm::vec2 size;
            
            // Interaction
            bool hasClosedCallback;
            std::function<void(const std::string&)> closedCallback;
            bool shouldClose;
            
            ImageButton* closeButton;
            Texture2D* closeTexture;
            
            TextButton* selectButton;
            
            TextField* nameTextField;
            
            VerticalLayout* hymnList;
            Texture2D* hymnTexture;
    };
}
