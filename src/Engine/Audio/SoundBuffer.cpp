#include "SoundBuffer.hpp"

#include "../Manager/SoundManager.hpp"
#include "SoundFile.hpp"
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"
#include "VorbisFile.hpp"

using namespace Audio;

SoundBuffer::SoundBuffer() {}

SoundBuffer::~SoundBuffer() {
#if !ANDROID
    if (soundFile != nullptr)
        alDeleteBuffers(1, &buffer);
#endif
}

#if !ANDROID
ALuint SoundBuffer::GetBuffer() const {
    return buffer;
}
#endif

void SoundBuffer::SetSoundFile(SoundFile* soundFile) {
#if !ANDROID
    // Remove old sound file.
    if (this->soundFile != nullptr) {
        alDeleteBuffers(1, &buffer);
    }

    // Create audio buffer.
    alGetError();
    alGenBuffers((ALuint)1, &buffer);
    SoundManager::CheckError("Couldn't create buffers.");

    // Set the buffer data.
    alBufferData(buffer, soundFile->GetFormat(), soundFile->GetData(), soundFile->GetSize(), soundFile->GetSampleRate());
    SoundManager::CheckError("Couldn't set buffer data.");
#endif
}

SoundFile* SoundBuffer::GetSoundFile() const {
    return soundFile;
}
