#pragma once

#include <cstddef>
#include <string>
#include <json/json.h>

namespace Audio {
    /// Ogg Vorbis sound file.
    /**
     * Loads .ogg-files.
     */
    class VorbisFile {
        public:
            /// Create new vorbis file.
            VorbisFile();
            
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
            const float* GetData() const;
            
            /// Get the length of the decoded audio data (per channel).
            /**
             * @return The length of the raw audio data.
             */
            std::size_t GetLength() const;
            
            /// Get whether the sound file is in stereo format.
            /**
             * @return Whether the sound file is in stereo format.
             */
            bool IsStereo() const;
            
            /// Get sample rate.
            /**
             * @return The sound file's sample rate (Hz).
             */
            int GetSampleRate() const;
            
            /// Save the sound.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const;
            
            /// Load sound from JSON node.
            /**
             * @param node JSON node to load from.
             */
            void Load(const Json::Value& node);
            
            /// Load sound from Ogg Vorbis file.
            /**
             * @param filename Filename (relative or absolute) to ogg-file.
             */
            void Load(const char* filename);
            
            /// The name of the sound.
            std::string name;
    
        private:
            float* audio;
            bool stereo;
            std::size_t length;
            int sampleRate;
    };
}
