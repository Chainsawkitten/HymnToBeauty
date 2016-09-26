#pragma once

#include <string>

class Font;
class Texture2D;
namespace Geometry {
    class Rectangle;
}

namespace GUI {
    class Label;
    class StringEditor;
    class Button;
    class FileSelector;
    class ImageTextButton;
    class BoolEditor;
    
    /// Used to edit a texture.
    class TextureEditor {
        public:
            /// Create new model editor.
            /**
             * @param fileSelector File selector to browse with.
             */
            TextureEditor(FileSelector* fileSelector);
            
            /// Destructor.
            ~TextureEditor();
            
            /// Show the editor.
            void Show();
            
            /// Set the texture to edit.
            /**
             * @param texture %Texture to edit.
             */
            void SetTexture(Texture2D* texture);
            
            /// Get whether the window is visible.
            /**
             * @return Whether the window is visible.
             */
            bool IsVisible() const;
            
            /// Set whether the window should be visible.
            /**
             * @param visible Whether the window should be visible.
             */
            void SetVisible(bool visible);
            
        private:
            void LoadPressed();
            void FileSelected(const std::string& file);
            
            Texture2D* texture = nullptr;
            bool visible = false;
            
            FileSelector* fileSelector;
            
            char name[128] = "";
    };
}
