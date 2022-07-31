#include "SoundSource.hpp"

#include "../Entity/Entity.hpp"
#include "../Hymn.hpp"
#include "../Audio/SoundFile.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Util/Json.hpp"

using namespace Component;

SoundSource::SoundSource() {
#if !ANDROID
    alGenSources(1, &source);
#endif

    soundBuffer = new Audio::SoundBuffer();
}

SoundSource::~SoundSource() {
#if !ANDROID
    alDeleteSources(1, &source);
#endif

    Audio::SoundFile* soundFile = soundBuffer->GetSoundFile();
    if (soundFile)
        Managers().resourceManager->FreeSound(soundFile);
    delete soundBuffer;
}

void SoundSource::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "sound", soundBuffer);
    Json::Serialize(node, load, "pitch", pitch, 1.0f);
    Json::Serialize(node, load, "gain", gain, 1.0f);
    Json::Serialize(node, load, "loop", loop, false);
}

void SoundSource::Play() {
    shouldPlay = true;
}

void SoundSource::Pause() {
    shouldPause = true;
}

void SoundSource::Stop() {
    shouldStop = true;
}
