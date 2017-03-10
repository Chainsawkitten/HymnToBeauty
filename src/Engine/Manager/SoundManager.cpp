#include "SoundManager.hpp"

#include "../Util/Log.hpp"
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Listener.hpp"
#include "../Component/SoundSource.hpp"
#include "Managers.hpp"

static const double sampleRate = 44100.0;

static int audioCallback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    Managers().soundManager->UpdateBuffer(static_cast<float*>(outputBuffer), framesPerBuffer);
    return 0;
}

SoundManager::SoundManager() {
    // Initialize PortAudio.
    PaError error = Pa_Initialize();
    if (error != paNoError)
        Log() << Pa_GetErrorText(error) << "\n";
    
    // Allocate buffer.
    int bufferSize = 256;
    leftBuffer = new float[bufferSize];
    rightBuffer = new float[bufferSize];
    
    // Open stream.
    error = Pa_OpenDefaultStream(&audioStream, 0, 2, paFloat32, sampleRate, bufferSize, audioCallback, nullptr);
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
    
    delete[] leftBuffer;
    delete[] rightBuffer;
}

void SoundManager::SetVolume(float volume) {
    /// @todo Set listener volume.
    this->volume = volume;
}

float SoundManager::GetVolume() const {
    return volume;
}

void SoundManager::Update(World& world) {
    this->world = &world;
    
    // Update sound sources.
    std::vector<Component::SoundSource*> soundComponents = world.GetComponents<Component::SoundSource>();
    for (Component::SoundSource* sound : soundComponents) {
        if (sound->IsKilled())
            continue;
        
        Entity* entity = sound->entity;
    }
    
    /// @todo Update listener.
}

void SoundManager::UpdateBuffer(float* outputBuffer, int bufferSize) {
    // Clear buffers.
    for (int i = 0; i < bufferSize; ++i) {
        leftBuffer[i] = 0.f;
        rightBuffer[i] = 0.f;
    }
    
    float frequency = 440.f;
    for (unsigned long i = 0; i < bufferSize; ++i) {
        // Left channel.
        *outputBuffer++ = 0.f; //0.1f * sin(i / sampleRate * frequency);
        
        // Right channel.
        *outputBuffer++ = 0.f;
    }
}
