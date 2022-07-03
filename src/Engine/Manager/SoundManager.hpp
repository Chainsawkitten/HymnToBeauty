#pragma once

#include <AL/alc.h>
#include "../Entity/ComponentContainer.hpp"

namespace Component {
class Listener;
class SoundSource;
} // namespace Component

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

    /// Moves sound sources and plays sounds.
    void Update();

    /// Create sound source component.
    /**
     * @return The created component.
     */
    Component::SoundSource* CreateSoundSource();

    /// Get all sound source components.
    /**
     * @return All sound source components.
     */
    const std::vector<Component::SoundSource*>& GetSoundSources() const;

    /// Create listener component.
    /**
     * @return The created component.
     */
    Component::Listener* CreateListener();

    /// Get all listener components.
    /**
     * @return All listener components.
     */
    const std::vector<Component::Listener*>& GetListeners() const;

    /// Remove all killed components.
    void ClearKilledComponents();

  private:
    SoundManager();
    ~SoundManager();
    SoundManager(SoundManager const&) = delete;
    void operator=(SoundManager const&) = delete;

    ALCdevice* device;
    ALCcontext* context;

    float volume = 1.f;

    ComponentContainer<Component::SoundSource> soundSources;
    ComponentContainer<Component::Listener> listeners;
};
