#pragma once

#include "SoundFile.hpp"

namespace Audio {
    /// Ogg Vorbis implementation of SoundFile.
    /**
     * Loads .ogg-files.
     */
    class VorbisFile : public SoundFile {
        public:
            /// Constructor.
            VorbisFile();
            
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
            
            /// Check whether sound is loaded.
            /**
             * @return Whether sound is loaded or not.
             */
            bool IsLoaded() const final;
            
        private:
            void Load(const char* filename);
            
            char* data = nullptr;
            int dataSize;
            int sampleRate;
            ALenum format;
    };
}
