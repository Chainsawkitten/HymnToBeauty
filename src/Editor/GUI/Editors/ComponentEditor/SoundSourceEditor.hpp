#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;
namespace Component {
    class SoundSource;
}
namespace Audio {
    class SoundBuffer;
}

namespace GUI {
    class FloatEditor;
    class BoolEditor;
    class TextButton;
    class SoundSelector;
    
    /// Used to edit a sound source component.
    class SoundSourceEditor : public ComponentEditor {
        public:
            /// Create new sound source editor.
            /**
             * @param parent Parent widget.
             * @param soundSelector Sound selector to use.
             */
            SoundSourceEditor(Widget* parent, SoundSelector* soundSelector);
            
            /// Destructor.
            ~SoundSourceEditor() override;
            
            /// Set the entity to edit the sound source component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            void SelectSoundPressed();
            void SoundSelected(Audio::SoundBuffer* sound);
            
            Font* font;
            
            FloatEditor* pitchEditor;
            FloatEditor* gainEditor;
            BoolEditor* loopEditor;
            
            Component::SoundSource* soundSource = nullptr;
            TextButton* selectSoundButton;
            
            SoundSelector* soundSelector;
    };
}
