#include "SoundManager.hpp"

#include <Utility/Log.hpp>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Listener.hpp"
#include "../Component/SoundSource.hpp"
#include "Managers.hpp"
#include "ResourceManager.hpp"

#define STB_VORBIS_HEADER_ONLY
#include <extras/stb_vorbis.c>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

// The stb_vorbis implementation must come after the implementation of miniaudio.
#undef STB_VORBIS_HEADER_ONLY
#include <extras/stb_vorbis.c>

SoundManager::SoundManager() {
    // Initialize miniaudio.
    ma_result result = ma_engine_init(nullptr, &engine);
    if (result != MA_SUCCESS) {
        Log(Log::ERR) << "Failed to initialize audio engine. Error code " << static_cast<int>(result) << "\n";
    }
}

SoundManager::~SoundManager() {
    ma_engine_uninit(&engine);
}

void SoundManager::Update() {
    // Update sound sources.
    for (Component::SoundSource* sound : soundSources.GetAll()) {
        if (sound->IsKilled() || !sound->entity->IsEnabled())
            continue;

        Entity* entity = sound->entity;

        // Pause it.
        if (sound->shouldPause) {
            ma_sound_stop(&sound->sound);
            sound->shouldPause = false;
        }

        // Stop it.
        if (sound->shouldStop) {
            ma_sound_stop(&sound->sound);
            ma_sound_seek_to_pcm_frame(&sound->sound, 0);
            sound->shouldStop = false;
        }

        // Set position.
        glm::vec3 position = entity->GetWorldPosition();
        ma_sound_set_position(&sound->sound, position.x, position.y, position.z);

        /// @todo Set velocity based on physics.

        // Set other properties.
        ma_sound_set_volume(&sound->sound, sound->gain);
        ma_sound_set_pitch(&sound->sound, sound->pitch);
        ma_sound_set_looping(&sound->sound, sound->loop);

        // Play it.
        if (sound->shouldPlay) {
            ma_sound_start(&sound->sound);
            sound->shouldPlay = false;
        }
    }

    // Update listener.
    for (Component::Listener* listener : listeners.GetAll()) {
        Entity* entity = listener->entity;

        // Set position
        glm::vec3 position = entity->GetWorldPosition();
        ma_engine_listener_set_position(&engine, 0, position.x, position.y, position.z);

        // Set orientation.
        glm::quat orientation = entity->GetWorldRotation();
        glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
        ma_engine_listener_set_direction(&engine, 0, forward.x, forward.y, forward.z);
        ma_engine_listener_set_world_up(&engine, 0, up.x, up.y, up.z);

        /// @todo More than one listener? Eg. for split-screen multiplayer.
        break;
    }
}

Component::SoundSource* SoundManager::CreateSoundSource() {
    Component::SoundSource* soundSource = soundSources.Create();
    soundSource->engine = &engine;
    return soundSource;
}

const std::vector<Component::SoundSource*>& SoundManager::GetSoundSources() const {
    return soundSources.GetAll();
}

Component::Listener* SoundManager::CreateListener() {
    return listeners.Create();
}

const std::vector<Component::Listener*>& SoundManager::GetListeners() const {
    return listeners.GetAll();
}

void SoundManager::ClearKilledComponents() {
    soundSources.ClearKilled();
    listeners.ClearKilled();
}
