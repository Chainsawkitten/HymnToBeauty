#include "EditorSettings.hpp"

#include <Engine/Util/FileSystem.hpp>

EditorSettings::EditorSettings() {
    ini.SetUnicode(true);
    ini.LoadFile(FileSystem::DataPath("Hymn to Beauty", "Settings.ini").c_str());
    
    AddBoolSetting("Logging", "Debug", "Logging", false);
    AddBoolSetting("Debug Context", "Debug", "Debug Context", false);
    
    AddLongSetting("Width", "Graphics", "Width", 800);
    AddLongSetting("Height", "Graphics", "Height", 600);
    AddStringSetting("Theme", "Graphics", "Theme", "Default");
    
    AddBoolSetting("Sound Source Icons", "View", "Sound Source Icons", true);
    AddBoolSetting("Particle Emitter Icons", "View", "Particle Emitter Icons", true);
    AddBoolSetting("Light Source Icons", "View", "Light Source Icons", true);
    AddBoolSetting("Camera Icons", "View", "Camera Icons", true);
    
    AddStringSetting("Text Editor", "Script", "Text Editor", "");
}

EditorSettings& EditorSettings::GetInstance() {
    static EditorSettings settings;
    
    return settings;
}

void EditorSettings::Save() {
    ini.SaveFile(FileSystem::DataPath("Hymn to Beauty", "Settings.ini").c_str());
}
