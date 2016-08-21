#include "SoundManager.hpp"

#include "../Util/Log.hpp"
#include <AL/al.h>

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
