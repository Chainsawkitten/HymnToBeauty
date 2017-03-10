#pragma once

#include "SoundFile.hpp"
#include <cstddef>

namespace Audio {
    /// Ogg Vorbis implementation of SoundFile.
    /**
     * Loads .ogg-files.
     */
    class VorbisFile : public SoundFile {
        public:
            /// Loads the specified ogg-file.
            /**
             * @param filename Filename (relative or absolute) to ogg-file.
             */
            VorbisFile(const char* filename);
    
            /// Destructor.
            ~VorbisFile();
    
            /// Get raw audio data.
            /**
             * @return Raw audio data.
             */
            const char* Data() const;
    
            /// Get data size.
            /**
             * @return The length of the raw audio data.
             */
            ALsizei Size() const;
    
            /// Get AL format.
            /**
             * 32-bit sound is not supported in OpenAL.
             * @return One of AL_FORMAT_MONO8, AL_FORMAT_MONO16, AL_FORMAT_STEREO8 or AL_FORMAT_STEREO16.
             */
            ALenum Format() const;
    
            /// Get sample rate.
            /**
             * @return The sound file's sample rate (Hz).
             */
            ALsizei SampleRate() const;
    
        private:
            char* data;
            float* audio;
            bool stereo;
            std::size_t length;
            int dataSize;
            int sampleRate;
            ALenum format;
    };
}
