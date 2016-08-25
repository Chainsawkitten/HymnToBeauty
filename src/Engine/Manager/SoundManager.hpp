#pragma once

#include <AL/alc.h>

class Scene;

/// Handles OpenAL sound.
class SoundManager {
    friend class Hub;
    
    public:
        /// Set main volume.
        /**
         * @param volume New volume.
         */
        void SetVolume(float volume);
        
        /// Get main volume.
        /**
         * @return The main volume.
         */
        float GetVolume() const;
        
        /// Check for OpenAL errors.
        /**
         * @param message Message to print to standard error if an error was encountered.
         */
        static void CheckError(const char* message);
        
        /// Update Scene containing entities. Moves sound sources and plays sounds.
        /**
         * @param scene The scene to update.
         */
        void Update(Scene& scene);
        
    private:
        SoundManager();
        ~SoundManager();
        SoundManager(SoundManager const&) = delete;
        void operator=(SoundManager const&) = delete;
        
        ALCdevice* device;
        ALCcontext* context;
        
        float volume = 1.f;
};
