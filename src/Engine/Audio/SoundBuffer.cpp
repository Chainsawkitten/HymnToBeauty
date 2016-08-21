#include "SoundBuffer.hpp"

#include "../Manager/SoundManager.hpp"
#include "SoundFile.hpp"

using namespace Audio;

SoundBuffer::SoundBuffer() {
    
}

SoundBuffer::SoundBuffer(SoundFile* soundFile) {
    Load(soundFile);
}

SoundBuffer::~SoundBuffer() {
    alDeleteBuffers(1, &buffer);
}

ALuint SoundBuffer::Buffer() const {
    return buffer;
}

void SoundBuffer::Load(SoundFile* soundFile) {
    // Create audio buffer.
    alGetError();
    alGenBuffers((ALuint)1, &buffer);
    SoundManager::CheckError("Couldn't create buffers.");
    
    // Set the buffer data.
    alBufferData(buffer, soundFile->Format(), soundFile->Data(), soundFile->Size(), soundFile->SampleRate());
    SoundManager::CheckError("Couldn't set buffer data.");
}
