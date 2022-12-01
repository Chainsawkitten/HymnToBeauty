#include "SoundSource.hpp"

#include "../Util/Json.hpp"
#include "../Hymn.hpp"
#include <Utility/Log.hpp>
#include "../Util/FileSystem.hpp"
#include <cassert>

using namespace Component;

SoundSource::SoundSource() {
    
}

SoundSource::~SoundSource() {
    FreeSound();
}

void SoundSource::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "sound", path, "");
    Json::Serialize(node, load, "pitch", pitch, 1.0f);
    Json::Serialize(node, load, "gain", gain, 1.0f);
    Json::Serialize(node, load, "loop", loop, false);

    if (load) {
        ReloadSound();
    }
}

void SoundSource::SetPath(const std::string& path) {
    this->path = path;
    ReloadSound();
}

const std::string& SoundSource::GetPath() const {
    return path;
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

void SoundSource::FreeSound() {
    if (soundInitialized) {
        ma_sound_uninit(&sound);
        soundInitialized = false;
    }
}

void SoundSource::ReloadSound() {
    assert(engine != nullptr);

    FreeSound();

    if (path.empty())
        return;

    const std::string fullPath = Hymn().GetPath() + FileSystem::DELIMITER + path;

    ma_sound_config config = ma_sound_config_init();
    config.pFilePath = fullPath.c_str();
    config.flags = MA_SOUND_FLAG_DECODE;
    config.isLooping = loop;

    ma_result result = ma_sound_init_ex(engine, &config, &sound);
    if (result != MA_SUCCESS) {
        Log(Log::ERR) << "Failed to load sound " << fullPath << ". Error code " << static_cast<int>(result) << "\n";
    }

    soundInitialized = true;
}
