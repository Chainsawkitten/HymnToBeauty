#include "SoundSource.hpp"

#include "../Entity/Entity.hpp"
#include "../Hymn.hpp"
#include "../Audio/VorbisFile.hpp"

using namespace Component;

SoundSource::SoundSource(Entity* entity) : SuperComponent(entity) {
    
}

SoundSource::~SoundSource() {
    
}

Json::Value SoundSource::Save() const {
    Json::Value component;
    
    if (vorbisFile != nullptr)
        component["sound"] = vorbisFile->name;
    
    component["gain"] = gain;
    component["loop"] = loop;
    return component;
}

void SoundSource::Load(const Json::Value& node) {
    std::string name = node.get("sound", "").asString();
    for (Audio::VorbisFile* sound : Hymn().sounds) {
        if (sound->name == name)
            vorbisFile = sound;
    }
    
    gain = node.get("gain", 1.f).asFloat();
    loop = node.get("loop", false).asBool();
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

float SoundSource::GetSample() {
    float sample = vorbisFile->GetData()[position];
    position = (position + 1) % (vorbisFile->GetLength() * (vorbisFile->IsStereo() ? 2 : 1));
    return sample;
}
