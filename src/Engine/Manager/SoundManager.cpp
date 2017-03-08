#include "SoundManager.hpp"

#include "../Util/Log.hpp"
#include <AL/al.h>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Listener.hpp"
#include "../Component/SoundSource.hpp"

static const double sampleRate = 44100.0;

static int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    float* out = static_cast<float*>(outputBuffer);
    
    float frequency = 440.f;
    
    for (unsigned long i = 0; i < framesPerBuffer; ++i) {
        // Left channel.
        *out++ = 0.1f * sin(i / sampleRate * frequency);
        
        // Right channel.
        *out++ = 0.f;
    }
    
    return 0;
}

SoundManager::SoundManager() {
    // Initialize PortAudio.
    PaError error = Pa_Initialize();
    if (error != paNoError)
        Log() << Pa_GetErrorText(error) << "\n";
    
    // Open stream.
    error = Pa_OpenDefaultStream(&audioStream, 0, 2, paFloat32, sampleRate, 256, audioCallback, nullptr);
    if (error != paNoError)
        Log() << Pa_GetErrorText(error) << "\n";
    
    // Start stream.
    error = Pa_StartStream(audioStream);
    if (error != paNoError)
        Log() << Pa_GetErrorText(error) << "\n";
}

SoundManager::~SoundManager() {
    // Abort and close stream.
    Pa_AbortStream(audioStream);
    Pa_CloseStream(audioStream);
    
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
