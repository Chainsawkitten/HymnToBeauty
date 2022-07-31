#pragma once

#if !ANDROID
#include <AL/al.h>
#endif
#include <string>
#include <json/json.h>

namespace Audio {
class SoundFile;

/// Wrapper for OpenAL buffers.
class SoundBuffer {
  public:
    /// Create new sound buffer.
    SoundBuffer();

    /// Destructor.
    ~SoundBuffer();

#if !ANDROID
    /// Get AL buffer.
    /**
     * @return The OpenAL buffer ID.
     */
    ALuint GetBuffer() const;
#endif

    /// Set sound file.
    /**
     * @param soundFile The %SoundFile containing the sound.
     */
    void SetSoundFile(SoundFile* soundFile);

    /// Get sound file.
    /**
     * @return The %SoundFile containing the sound.
     */
    SoundFile* GetSoundFile() const;

  private:
    SoundFile* soundFile = nullptr;
#if !ANDROID
    ALuint buffer = 0;
#endif
};
} // namespace Audio
