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
    class VerticalScrollLayout;
    
    /// A window where a file can be selected.
    class FileSelector : public Container {
        public:
            /// Create new window.
            /**
             * @param parent Parent widget.
             */
            FileSelector(Widget* parent);
            
            /// Destructor.
            ~FileSelector() override;
            
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
            
            /// Set function to call when a file has been selected.
            /**
             * @param callback Function to call.
             */
            void SetFileSelectedCallback(std::function<void(const std::string&)> callback);
            
            /// Set extension to select files of.
            /**
             * @param extension Set the extension of which files to show.
             */
            void SetExtension(const std::string& extension);
            
        private:
            void Close();
            void OpenParentDirectory();
            void OpenDirectory(const std::string& name);
            void SelectFile(const std::string& name);
            void ScanDirectory();
            
            Geometry::Rectangle* rectangle;
            Font* font;
            
            glm::vec2 size;
            
            // Interaction
            std::string path;
            std::string extension = "";
            bool pathChanged;
            std::string file = "";
            
            bool hasFileSelectedCallback = false;
            std::function<void(const std::string&)> fileSelectedCallback;
            
            ImageButton* closeButton;
            Texture2D* closeTexture;
            
            VerticalScrollLayout* fileList;
            Texture2D* directoryTexture;
            Texture2D* fileTexture;
    };
}
