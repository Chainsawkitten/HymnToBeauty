#pragma once

#include "../Widget.hpp"
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
    class TextureEditor : public Widget {
        public:
            /// Create new model editor.
            /**
             * @param parent Parent widget.
             * @param fileSelector File selector to browse with.
             */
            TextureEditor(Widget* parent, FileSelector* fileSelector);
            
            /// Destructor.
            ~TextureEditor();
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            void Render() override;
            
            /// Show the editor.
            void Show();
            
            /// Set widget's position.
            /**
             * @param position New position.
             */
            void SetPosition(const glm::vec2& position) override;
            
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
            
            /// Set the texture to edit.
            /**
             * @param texture %Texture to edit.
             */
            void SetTexture(Texture2D* texture);
            
        private:
            void DeleteTexturePressed();
            void LoadPressed();
            void FileSelected(const std::string& file);
            
            Geometry::Rectangle* rectangle;
            glm::vec2 size;
            Font* font;
            
            Texture2D* texture = nullptr;
            
            Label* nameLabel;
            StringEditor* nameEditor;
            
            Texture2D* deleteTextureTexture;
            ImageTextButton* deleteTextureButton;
            
            Button* loadButton;
            FileSelector* fileSelector;
            
            Label* srgbLabel;
            BoolEditor* srgbEditor;
            
            char name[128] = "";
    };
}
