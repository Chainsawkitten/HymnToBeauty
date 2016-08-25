#pragma once

#include "Container.hpp"
#include <functional>

class Texture2D;
class Font;
namespace Geometry {
    class Rectangle;
}
namespace Audio {
    class SoundBuffer;
}

namespace GUI {
    class ImageButton;
    class VerticalScrollLayout;
    
    /// A window where a sound can be selected.
    class SoundSelector : public Container {
        public:
            /// Create new window.
            /**
             * @param parent Parent widget.
             */
            SoundSelector(Widget* parent);
            
            /// Destructor.
            ~SoundSelector() override;
            
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
            
            /// Set function to call when a sound has been selected.
            /**
             * @param callback Function to call.
             */
            void SetSoundSelectedCallback(std::function<void(Audio::SoundBuffer*)> callback);
            
            /// Set the sounds to select from.
            /**
             * @param sounds The sounds to select from.
             */
            void SetSounds(const std::vector<Audio::SoundBuffer*>* sounds);
            
            /// Update the list of sounds.
            void UpdateSounds();
            
        private:
            void Close();
            void SoundSelected(Audio::SoundBuffer* sound);
            
            Geometry::Rectangle* rectangle;
            Font* font;
            
            glm::vec2 size;
            
            bool hasSoundSelectedCallback = false;
            std::function<void(Audio::SoundBuffer*)> soundSelectedCallback;
            
            ImageButton* closeButton;
            Texture2D* closeTexture;
            
            const std::vector<Audio::SoundBuffer*>* sounds;
            VerticalScrollLayout* soundList;
    };
}
