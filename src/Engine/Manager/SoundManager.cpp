#include "SoundManager.hpp"

#include "../Util/Log.hpp"
#include <AL/al.h>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Listener.hpp"
#include "../Component/SoundSource.hpp"
#include "../Component/Physics.hpp"
#include "../Audio/SoundBuffer.hpp"

// Scaling constant. Used to convert from our units to sound system units.
const float soundScale = 0.2f;

SoundManager::SoundManager() {
    // Open default audio device.
    device = alcOpenDevice(nullptr);
    if (!device)
        Log() << "Couldn't open default audio device.\n";
    
    // Create audio context.
    context = alcCreateContext(device, nullptr);
    if (!alcMakeContextCurrent(context))
        Log() << "Couldn't create audio context.\n";
}

SoundManager::~SoundManager() {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void SoundManager::SetVolume(float volume) {
    alListenerf(AL_GAIN, volume);
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
            alSourcePause(sound->source);
            sound->shouldPause = false;
        }
        
        // Stop it.
        if (sound->shouldStop) {
            alSourceStop(sound->source);
            sound->shouldStop = false;
        }
        
        // Set position.
        glm::vec3 position = soundScale * glm::vec3(entity->GetModelMatrix() * glm::vec4(0.f, 0.f, 0.f, 1.f));
        alSource3f(sound->source, AL_POSITION, position.x, position.y, position.z);
        
        // Set velocity based on physics.
        Component::Physics* physics = entity->GetComponent<Component::Physics>();
        if (physics != nullptr) {
            glm::vec3 velocity = soundScale * physics->velocity;
            alSource3f(sound->source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
        } else {
            alSource3f(sound->source, AL_VELOCITY, 0.f, 0.f, 0.f);
        }
        
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
    std::vector<Component::Listener*> listeners = world.GetComponents<Component::Listener>();
    for (Component::Listener* listener : listeners) {
        Entity* entity = listener->entity;
        
        // Set position
        glm::vec3 position = soundScale * entity->position;
        alListener3f(AL_POSITION, position.x, position.y, position.z);
        CheckError("Couldn't set listener position.");
        
        // Set orientation.
        glm::vec4 forward = glm::inverse(entity->GetOrientation()) * glm::vec4(0.f, 0.f, -1.f, 1.f);
        glm::vec4 up = glm::inverse(entity->GetOrientation()) * glm::vec4(0.f, 1.f, 0.f, 1.f);
        ALfloat listenerOri[] = { forward.x, forward.y, forward.z, up.x, up.y, up.z };
        alListenerfv(AL_ORIENTATION, listenerOri);
        CheckError("Couldn't set listener orientation.");
        
        break;
    }
}
