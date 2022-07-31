#pragma once

#include "SoundFile.hpp"

namespace Audio {
/// Ogg Vorbis implementation of SoundFile.
/**
 * Loads .ogg-files.
 */
class VorbisFile : public SoundFile {
  public:
    /// Constructor.
    VorbisFile();

    /// Destructor.
    ~VorbisFile() final;

    const char* GetData() const final;
#if !ANDROID
    ALsizei GetSize() const final;
    ALenum GetFormat() const final;
    ALsizei GetSampleRate() const final;
#endif
    bool IsLoaded() const final;

  private:
    void Load(const char* filename);

    char* data = nullptr;
    int dataSize;
    int sampleRate;
#if !ANDROID
    ALenum format;
#endif
};
} // namespace Audio
