#include "EditorSettings.hpp"

#include <Core/Util/FileSystem.hpp>

EditorSettings::EditorSettings() {
    ini.SetUnicode(true);
    ini.LoadFile(FileSystem::SavePath("Hymn to Beauty", "Settings.ini").c_str());
    
    AddBoolSetting("Logging", "Debug", "Logging", false);
}

EditorSettings& EditorSettings::GetInstance() {
    static EditorSettings settings;
    
    return settings;
}

void EditorSettings::Save() {
    ini.SaveFile(FileSystem::SavePath("Hymn to Beauty", "Settings.ini").c_str());
}
