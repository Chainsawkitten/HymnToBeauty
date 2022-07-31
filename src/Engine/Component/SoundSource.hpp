#pragma once

#include "SuperComponent.hpp"
#if !ANDROID
#include <AL/al.h>
#endif

class SoundManager;
namespace Audio {
class SoundBuffer;
}

namespace Component {
/// %Component describing a sound source.
class SoundSource : public SuperComponent {
    friend class ::SoundManager;

  public:
    /// Create new sound source.
    SoundSource();

    /// Destructor.
    ~SoundSource();

    void Serialize(Json::Value& node, bool load) override;

    /// Play the sound.
    void Play();

    /// Pause the sound.
    void Pause();

    /// Stop the sound.
    void Stop();

    /// Sound buffer.
    Audio::SoundBuffer* soundBuffer = nullptr;

    /// Pitch.
    float pitch = 1.f;

    /// Gain.
    float gain = 1.f;

    /// Whether the sound should loop.
    bool loop = false;

  private:
#if !ANDROID
    ALuint source;
#endif

    bool shouldPlay = false;
    bool shouldPause = false;
    bool shouldStop = false;

    bool soundBufferSet = false;
};
} // namespace Component
