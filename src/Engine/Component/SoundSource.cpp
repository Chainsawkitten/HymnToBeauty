#include "SoundSource.hpp"

#include "../Entity/Entity.hpp"
#include "../Hymn.hpp"
#include "../Audio/SoundFile.hpp"
#include "../Audio/SoundBuffer.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"

using namespace Component;

SoundSource::SoundSource() {
    alGenSources(1, &source);
    
    soundBuffer = new Audio::SoundBuffer();
}

SoundSource::~SoundSource() {
    alDeleteSources(1, &source);
    
    Audio::SoundFile* soundFile = soundBuffer->GetSoundFile();
    if (soundFile)
        Managers().resourceManager->FreeSound(soundFile);
    delete soundBuffer;
}

Json::Value SoundSource::Save() const {
    Json::Value component;
    
    Audio::SoundFile* soundFile = soundBuffer->GetSoundFile();
    if (soundFile)
        component["sound"] = soundFile->path + soundFile->name;
    
    component["pitch"] = pitch;
    component["gain"] = gain;
    component["loop"] = loop;
    return component;
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
