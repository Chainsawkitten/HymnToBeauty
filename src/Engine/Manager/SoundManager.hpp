#pragma once

#include "../Entity/ComponentContainer.hpp"
#include <miniaudio.h>

namespace Component {
class Listener;
class SoundSource;
} // namespace Component

/// Handles sound.
class SoundManager {
    friend class Hub;

  public:
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

    ma_engine engine;

    ComponentContainer<Component::SoundSource> soundSources;
    ComponentContainer<Component::Listener> listeners;
};
