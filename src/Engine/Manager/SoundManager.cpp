#include "SoundManager.hpp"

#include "../Util/Log.hpp"
#include <AL/al.h>
#include <portaudio.h>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Listener.hpp"
#include "../Component/SoundSource.hpp"

SoundManager::SoundManager() {
    // Initialize PortAudio.
    PaError error = Pa_Initialize();
    if (error != paNoError)
        Log() << Pa_GetErrorText(error) << "\n";
}

SoundManager::~SoundManager() {
    // Terminate PortAudio.
    PaError error = Pa_Terminate();
    if (error != paNoError)
        Log() << Pa_GetErrorText(error) << "\n";
}

void SoundManager::SetVolume(float volume) {
    /// @todo Set listener volume.
    this->volume = volume;
}

float SoundManager::GetVolume() const {
    return volume;
}

void SoundManager::CheckError(const char* message) {
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        Log() << message << "\n";
        if (error == AL_INVALID_NAME) Log() << "Invalid name\n";
        if (error == AL_INVALID_ENUM) Log() << "Invalid enum\n";
        if (error == AL_INVALID_VALUE) Log() << "Invalid value\n";
        if (error == AL_INVALID_OPERATION) Log() << "Invalid operation\n";
        if (error == AL_OUT_OF_MEMORY) Log() << "Out of memory like!\n";
    }
}

void SoundManager::Update(World& world) {
    // Update sound sources.
    std::vector<Component::SoundSource*> soundComponents = world.GetComponents<Component::SoundSource>();
    for (Component::SoundSource* sound : soundComponents) {
        if (sound->IsKilled())
            continue;
        
        Entity* entity = sound->entity;
        
        // Pause it.
        if (sound->shouldPause) {
            /// @todo Pause it.
            sound->shouldPause = false;
        }
        
        // Stop it.
        if (sound->shouldStop) {
            /// @todo Stop it.
            sound->shouldStop = false;
        }
        
        /// @todo Set position.
        
        // Play it.
        if (sound->shouldPlay) {
            /// @todo Play it.
            sound->shouldPlay = false;
        }
    }
    
    /// @todo Update listener.
}
