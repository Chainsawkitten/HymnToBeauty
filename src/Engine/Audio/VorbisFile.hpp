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
    ALsizei GetSize() const final;
    ALenum GetFormat() const final;
    ALsizei GetSampleRate() const final;
    bool IsLoaded() const final;

  private:
    void Load(const char* filename);

    char* data = nullptr;
    int dataSize;
    int sampleRate;
    ALenum format;
};
} // namespace Audio
