#pragma once

#include "Container.hpp"
#include <functional>

class Texture2D;
class Font;
namespace Geometry {
    class Rectangle;
}

namespace GUI {
    class ImageButton;
    class TextButton;
    class VerticalLayout;
    class TextField;
    
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
            bool hasClosedCallback = false;
            std::function<void(const std::string&)> closedCallback;
            bool shouldClose = false;
            
            ImageButton* closeButton;
            Texture2D* closeTexture;
            
            TextButton* selectButton;
            
            TextField* nameTextField;
            
            VerticalLayout* hymnList;
            Texture2D* hymnTexture;
    };
}
