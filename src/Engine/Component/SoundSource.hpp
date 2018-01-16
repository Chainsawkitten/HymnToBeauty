#pragma once

#include "SuperComponent.hpp"
#include <AL/al.h>

class SoundManager;
namespace Audio {
    class SoundBuffer;
}

namespace Component {
    /// %Component describing a sound source.
    class SoundSource : public SuperComponent {
        friend class ::SoundManager;
        
        public:
            /// Create new sound source.
            SoundSource();
            
            /// Destructor.
            ~SoundSource();
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;
            
            /// Play the sound.
            void Play();
            
            /// Pause the sound.
            void Pause();
            
            /// Stop the sound.
            void Stop();
            
            /// Sound buffer.
            Audio::SoundBuffer* soundBuffer = nullptr;
            
            /// Pitch.
            float pitch = 1.f;
            
            /// Gain.
            float gain = 1.f;
            
            /// Whether the sound should loop.
            bool loop = false;
            
        private:
            ALuint source;
            
            bool shouldPlay = false;
            bool shouldPause = false;
            bool shouldStop = false;
            
            bool soundBufferSet = false;
    };
}
