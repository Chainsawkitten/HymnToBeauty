#include "SoundSource.hpp"

#include "../Entity/Entity.hpp"
#include "../Hymn.hpp"
#include "../Audio/SoundBuffer.hpp"

using namespace Component;

SoundSource::SoundSource(Entity* entity) : SuperComponent(entity) {
    alGenSources(1, &source);
}

SoundSource::~SoundSource() {
    alDeleteSources(1, &source);
}

Json::Value SoundSource::Save() const {
    Json::Value component;
    
    if (soundBuffer != nullptr)
        component["sound"] = soundBuffer->name;
    
    component["pitch"] = pitch;
    component["gain"] = gain;
    component["loop"] = loop;
    return component;
}

void SoundSource::Load(const Json::Value& node) {
    std::string name = node.get("sound", "").asString();
    for (Audio::SoundBuffer* s : Hymn().sounds) {
        if (s->name == name)
            soundBuffer = s;
    }
    
    pitch = node.get("pitch", 1.f).asFloat();
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
