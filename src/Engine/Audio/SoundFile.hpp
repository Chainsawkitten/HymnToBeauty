#pragma once

#include <AL/al.h>
#include <string>

namespace Audio {
    /// Interface for sound files of various formats.
    /**
     * Used to get raw audio data to a SoundBuffer.
     */
    class SoundFile {
        public:
            /// Destructor.
            virtual ~SoundFile();

            /// Get raw audio data.
            /**
             * @return Raw audio data.
             */
            virtual const char* GetData() const = 0;

            /// Get data size.
            /**
             * @return The length of the raw audio data.
             */
            virtual ALsizei GetSize() const = 0;

            /// Get AL format.
            /**
             * 32-bit sound is not supported in OpenAL.
             * @return One of AL_FORMAT_MONO8, AL_FORMAT_MONO16, AL_FORMAT_STEREO8 or AL_FORMAT_STEREO16.
             */
            virtual ALenum GetFormat() const = 0;

            /// Get sample rate.
            /**
             * @return The sound file's sample rate (Hz).
             */
            virtual ALsizei GetSampleRate() const = 0;

            /// Check whether sound is loaded.
            /**
             * @return Whether sound is loaded or not.
             */
            virtual bool IsLoaded() const = 0;

            /// Save sound.
            void Save() const;

            /// Load sound.
            /**
             * @param name Name of the sound file on disk.
             */
            void Load(const std::string& name);

            /// The name of the sound.
            std::string name;

            /// The folder containing the sound file.
            std::string path;

        private:
            virtual void Load(const char* filename) = 0;
    };
}
