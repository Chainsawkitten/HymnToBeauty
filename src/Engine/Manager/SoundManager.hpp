#pragma once

#include <portaudio.h>

class World;

/// Handles sound.
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
        
        /// Update world containing entities. Moves sound sources and plays sounds.
        /**
         * @param world The world to update.
         */
        void Update(World& world);
        
    private:
        SoundManager();
        ~SoundManager();
        SoundManager(SoundManager const&) = delete;
        void operator=(SoundManager const&) = delete;
        
        PaStream* audioStream;
        
        float volume = 1.f;
};
