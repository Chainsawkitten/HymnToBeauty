#pragma once

#include "../Widget.hpp"
#include <string>

class Font;
class Texture2D;
namespace Geometry {
    class Rectangle;
}
namespace Audio {
    class SoundBuffer;
}

namespace GUI {
    class Label;
    class StringEditor;
    class Button;
    class FileSelector;
    class ImageTextButton;
    
    /// Used to edit a sound.
    class SoundEditor : public Widget {
        public:
            /// Create new sound editor.
            /**
             * @param parent Parent widget.
             * @param fileSelector File selector to browse with.
             */
            SoundEditor(Widget* parent, FileSelector* fileSelector);
            
            /// Destructor.
            ~SoundEditor();
            
            /// Update the widget.
            void Update() override;
            
            /// Render the widget.
            void Render() override;
            
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
            
            /// Set the sound to edit.
            /**
             * @param sound Sound to edit.
             */
            void SetSound(Audio::SoundBuffer* sound);
            
        private:
            void DeleteSoundPressed();
            void LoadPressed();
            void FileSelected(const std::string& file);
            
            Geometry::Rectangle* rectangle;
            glm::vec2 size;
            Font* font;
            
            Audio::SoundBuffer* sound = nullptr;
            
            Label* nameLabel;
            StringEditor* nameEditor;
            
            Texture2D* deleteSoundTexture;
            ImageTextButton* deleteSoundButton;
            
            Button* loadButton;
            FileSelector* fileSelector;
    };
}
