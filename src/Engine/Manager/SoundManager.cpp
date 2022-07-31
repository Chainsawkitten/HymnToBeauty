#include "SoundManager.hpp"

#if !ANDROID
#include <AL/al.h>
#endif
#include <Utility/Log.hpp>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Listener.hpp"
#include "../Component/Mesh.hpp"
#include "../Component/SoundSource.hpp"
#include "../Audio/SoundFile.hpp"
#include "../Audio/SoundBuffer.hpp"
#include <Video/Geometry/Geometry3D.hpp>
#include "Managers.hpp"
#include "ResourceManager.hpp"
#include <cstdint>
#include <cstring>
#include <algorithm>

using namespace Audio;

SoundManager::SoundManager() {
#if ANDROID
    Log(Log::ERR) << "Sound not implemented on Android.";
#else
    // Open default audio device.
    device = alcOpenDevice(nullptr);
    if (!device)
        Log() << "Couldn't open default audio device.\n";

    // Create audio context.
    context = alcCreateContext(device, nullptr);
    if (!alcMakeContextCurrent(context))
        Log() << "Couldn't create audio context.\n";
#endif
}

SoundManager::~SoundManager() {
#if !ANDROID
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
#endif
}

void SoundManager::SetVolume(float volume) {
#if !ANDROID
    alListenerf(AL_GAIN, volume);
#endif
    this->volume = volume;
}

float SoundManager::GetVolume() const {
    return volume;
}

void SoundManager::CheckError(const char* message) {
#if !ANDROID
    ALenum error = alGetError();
    if (error != AL_NO_ERROR) {
        Log() << message << "\n";
        if (error == AL_INVALID_NAME)
            Log() << "Invalid name\n";
        if (error == AL_INVALID_ENUM)
            Log() << "Invalid enum\n";
        if (error == AL_INVALID_VALUE)
            Log() << "Invalid value\n";
        if (error == AL_INVALID_OPERATION)
            Log() << "Invalid operation\n";
        if (error == AL_OUT_OF_MEMORY)
            Log() << "Out of memory like!\n";
    }
#endif
}

void SoundManager::Update() {
#if !ANDROID
    // Update sound sources.
    for (Component::SoundSource* sound : soundSources.GetAll()) {
        if (sound->IsKilled() || !sound->entity->IsEnabled())
            continue;

        Entity* entity = sound->entity;

        // Pause it.
        if (sound->shouldPause) {
            alSourcePause(sound->source);
            sound->shouldPause = false;
        }

        // Stop it.
        if (sound->shouldStop) {
            alSourceStop(sound->source);
            sound->shouldStop = false;
        }

        // Set position.
        glm::vec3 position = glm::vec3(entity->GetModelMatrix() * glm::vec4(0.f, 0.f, 0.f, 1.f));
        alSource3f(sound->source, AL_POSITION, position.x, position.y, position.z);

        /// @todo Set velocity based on physics.

        // Set other properties.
        alSourcef(sound->source, AL_PITCH, sound->pitch);
        alSourcef(sound->source, AL_GAIN, sound->gain);
        alSourcei(sound->source, AL_LOOPING, sound->loop);
        if (sound->soundBuffer != nullptr && !sound->soundBufferSet) {
            alSourcei(sound->source, AL_BUFFER, sound->soundBuffer->GetBuffer());
            sound->soundBufferSet = true;
        }

        // Play it.
        if (sound->shouldPlay) {
            alSourcePlay(sound->source);
            sound->shouldPlay = false;
        }

        CheckError("Something went wrong updating a sound source.");
    }

    // Update listener.
    for (Component::Listener* listener : listeners.GetAll()) {
        Entity* entity = listener->entity;

        // Set position
        glm::vec3 position = entity->position;
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        CheckError("Couldn't set listener position.");

        // Set orientation.
        glm::quat orientation = entity->GetWorldOrientation();
        glm::vec3 forward = orientation * glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 up = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
        ALfloat listenerOri[] = {forward.x, forward.y, forward.z, up.x, up.y, up.z};
        alListenerfv(AL_ORIENTATION, listenerOri);
        CheckError("Couldn't set listener orientation.");

        break;
    }
#endif
}

Component::SoundSource* SoundManager::CreateSoundSource() {
    return soundSources.Create();
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
