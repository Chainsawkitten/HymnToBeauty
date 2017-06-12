#pragma once

#include "SoundFile.hpp"

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
            ~VorbisFile() final;
            
            /// Get raw audio data.
            /**
             * @return Raw audio data.
             */
            const char* GetData() const final;
            
            /// Get data size.
            /**
             * @return The length of the raw audio data.
             */
            ALsizei GetSize() const final;
            
            /// Get AL format.
            /**
             * 32-bit sound is not supported in OpenAL.
             * @return One of AL_FORMAT_MONO8, AL_FORMAT_MONO16, AL_FORMAT_STEREO8 or AL_FORMAT_STEREO16.
             */
            ALenum GetFormat() const final;
            
            /// Get sample rate.
            /**
             * @return The sound file's sample rate (Hz).
             */
            ALsizei GetSampleRate() const final;
            
        private:
            char* data;
            int dataSize;
            int sampleRate;
            ALenum format;
    };
}
