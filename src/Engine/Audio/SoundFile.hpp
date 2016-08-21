#pragma once

#include <AL/al.h>

namespace Audio {
    /// Interface for sound files of various formats.
    /**
     * Used to get raw audio data to a SoundBuffer.
     */
    class SoundFile {
        public:
            /// Destructor.
            virtual ~SoundFile() { }
    
            /// Get raw audio data.
            /**
             * @return Raw audio data.
             */
            virtual const char* Data() const = 0;
    
            /// Get data size.
            /**
             * @return The length of the raw audio data.
             */
            virtual ALsizei Size() const = 0;
    
            /// Get AL format.
            /**
             * 32-bit sound is not supported in OpenAL.
             * @return One of AL_FORMAT_MONO8, AL_FORMAT_MONO16, AL_FORMAT_STEREO8 or AL_FORMAT_STEREO16.
             */
            virtual ALenum Format() const = 0;
    
            /// Get sample rate.
            /**
             * @return The sound file's sample rate (Hz).
             */
            virtual ALsizei SampleRate() const = 0;
    };
}
