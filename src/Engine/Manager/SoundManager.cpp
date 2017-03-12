#include "SoundManager.hpp"

#include "../Util/Log.hpp"
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/Listener.hpp"
#include "../Component/SoundSource.hpp"
#include "../Audio/VorbisFile.hpp"
#include "Managers.hpp"
#include <algorithm>

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
    
    /// @todo Update listener.
}

void SoundManager::UpdateBuffer(float* outputBuffer, int bufferSize) {
    // Clear buffers.
    for (int i = 0; i < bufferSize; ++i) {
        leftBuffer[i] = 0.f;
        rightBuffer[i] = 0.f;
    }
    
    if (world != nullptr) {
        // Get listener.
        const std::vector<Component::Listener*>& listeners = world->GetComponents<Component::Listener>();
        if (listeners.size() > 0) {
            Component::Listener* listener = listeners[0];
            
            // Play sound sources.
            const std::vector<Component::SoundSource*>& soundComponents = world->GetComponents<Component::SoundSource>();
            for (Component::SoundSource* sound : soundComponents) {
                if (sound->IsKilled() || !sound->playing)
                    continue;
                
                if (sound->vorbisFile != nullptr) {
                    if (sound->vorbisFile->IsStereo()) {
                        // Stereo sound (no 3D effects).
                        for (int i = 0; i < bufferSize; ++i) {
                            leftBuffer[i] += sound->gain * sound->GetSample();
                            rightBuffer[i] += sound->gain * sound->GetSample();
                        }
                    } else {
                        // Mono sound (3D positional).
                        Entity* entity = sound->entity;
                        
                        // Attenuation.
                        glm::vec3 direction = entity->position - listener->entity->position;
                        float distance = glm::length(direction);
                        float attenuation = 0.5f;
                        float minDistance = 1.f;
                        float volume = sound->gain * minDistance / (minDistance + attenuation * ((std::max)(distance, minDistance) - minDistance));
                        
                        // Panning.
                        glm::mat4 orientation = listener->entity->GetCameraOrientation();
                        glm::vec3 right = glm::vec4(1.f, 0.f, 0.f, 0.f) * orientation;
                        direction /= distance;
                        float pan = glm::dot(right, direction);
                        float leftVolume = volume * (1.f - pan) * 0.5f;
                        float rightVolume = volume * (1.f + pan) * 0.5f;
                        
                        for (int i = 0; i < bufferSize; ++i) {
                            float sample = sound->GetSample();
                            leftBuffer[i] += leftVolume * sample;
                            rightBuffer[i] += rightVolume * sample;
                        }
                    }
                }
            }
        }
    }
    
    // Copy final buffer contents.
    for (unsigned long i = 0; i < bufferSize; ++i) {
        // Left channel.
        *outputBuffer++ = leftBuffer[i];
        
        // Right channel.
        *outputBuffer++ = rightBuffer[i];
    }
}
