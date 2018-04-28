#include "GameSettings.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <Engine/Hymn.hpp>

GameSettings::GameSettings() {
}

GameSettings& GameSettings::GetInstance() {
    static GameSettings settings;

    return settings;
}

void GameSettings::Save() {
    ini.SaveFile(FileSystem::DataPath(Hymn().name.c_str(), "Settings.ini").c_str());
}

void GameSettings::Load() {
    ini.SetUnicode(true);
    ini.LoadFile(FileSystem::DataPath(Hymn().name.c_str(), "Settings.ini").c_str());

    AddLongSetting("Texture Reduction", "Graphics", "Texture Reduction", 1);
    AddLongSetting("Shadow Map Size", "Graphics", "Shadow Map Size", 1024);
}
