#pragma once

#include "SuperComponent.hpp"
#include <string>
#include <miniaudio.h>

class SoundManager;

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

    /// Set the path to the sound file to play.
    /**
     * @parma path Path to the sound file to play.
     */
    void SetPath(const std::string& path);

    /// Get the path to the sound file to play.
    /**
     * @return The path to the sound file to play.
     */
    const std::string& GetPath() const;

    /// Play the sound.
    void Play();

    /// Pause the sound.
    void Pause();

    /// Stop the sound.
    void Stop();

    /// Pitch.
    float pitch = 1.0f;

    /// Gain.
    float gain = 1.0f;

    /// Whether the sound should loop.
    bool loop = false;

  private:
    void FreeSound();
    void ReloadSound();

    ma_engine* engine;
    ma_sound sound;
    std::string path = "";

    bool shouldPlay = false;
    bool shouldPause = false;
    bool shouldStop = false;

    bool soundInitialized = false;
};
} // namespace Component
