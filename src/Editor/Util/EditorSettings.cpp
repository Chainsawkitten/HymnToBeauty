#include "EditorSettings.hpp"

#include <Engine/Util/FileSystem.hpp>

EditorSettings::EditorSettings() {
    ini.SetUnicode(true);
    ini.LoadFile(FileSystem::DataPath("Hymn to Beauty", "Settings.ini").c_str());
    
    AddBoolSetting("Logging", "Debug", "Logging", false);
    AddBoolSetting("Debug Context", "Debug", "Debug Context", false);
}

EditorSettings& EditorSettings::GetInstance() {
    static EditorSettings settings;
    
    return settings;
}

void EditorSettings::Save() {
    ini.SaveFile(FileSystem::DataPath("Hymn to Beauty", "Settings.ini").c_str());
}
