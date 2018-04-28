#include "SoundBuffer.hpp"

#include "../Manager/SoundManager.hpp"
#include "SoundFile.hpp"
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"
#include "VorbisFile.hpp"

using namespace Audio;

SoundBuffer::SoundBuffer() {
}

SoundBuffer::~SoundBuffer() {
    if (soundFile != nullptr)
        alDeleteBuffers(1, &buffer);
}

ALuint SoundBuffer::GetBuffer() const {
    return buffer;
}

void SoundBuffer::SetSoundFile(SoundFile* soundFile) {
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
}

SoundFile* SoundBuffer::GetSoundFile() const {
    return soundFile;
}
