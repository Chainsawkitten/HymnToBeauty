#pragma once

#include <AL/al.h>
#include <string>
#include <json/json.h>

namespace Audio {
    class SoundFile;
    
    /// Wrapper for OpenAL buffers.
    class SoundBuffer {
        public:
            /// Create new sound buffer.
            SoundBuffer();
            
            /// Destructor.
            ~SoundBuffer();
            
            /// Get AL buffer.
            /**
             * @return The OpenAL buffer ID.
             */
            ALuint GetBuffer() const;
            
            /// Set sound file.
            /**
             * @param soundFile The %SoundFile containing the sound.
             */
            void SetSoundFile(SoundFile* soundFile);
            
            /// Get sound file.
            /**
             * @return The %SoundFile containing the sound.
             */
            SoundFile* GetSoundFile() const;
            
        private:
            SoundFile* soundFile = nullptr;
            ALuint buffer = 0;
    };
}
