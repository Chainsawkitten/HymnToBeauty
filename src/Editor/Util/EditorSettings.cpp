#include "EditorSettings.hpp"

#include <Engine/Util/FileSystem.hpp>

EditorSettings::EditorSettings() {
    ini.SetUnicode(true);
    ini.LoadFile(FileSystem::DataPath("Hymn to Beauty", "Settings.ini").c_str());

    AddBoolSetting("Logging", "Debug", "Logging", false);

    AddLongSetting("Width", "Graphics", "Width", 800);
    AddLongSetting("Height", "Graphics", "Height", 600);
    AddStringSetting("Theme", "Graphics", "Theme", "Default");
    AddStringSetting("API", "Graphics", "API", "OpenGL");

    AddBoolSetting("Sound Source Icons", "View", "Sound Source Icons", true);
    AddBoolSetting("Light Source Icons", "View", "Light Source Icons", true);
    AddBoolSetting("Camera Icons", "View", "Camera Icons", true);
    AddBoolSetting("Physics Volumes", "View", "Physics Volumes", true);
    AddBoolSetting("Lighting", "View", "Lighting", true);
    AddBoolSetting("LightVolumes", "View", "Light Volumes", false);

    AddStringSetting("Text Editor", "Script", "Text Editor", "");

    AddBoolSetting("Grid Settings", "View", "Grid Settings", false);
    AddLongSetting("Grid Size", "Grid", "Size", 100);
    AddLongSetting("Grid Line Width", "Grid", "Line Width", 3);
    AddBoolSetting("Grid Snap", "Grid", "Snap", false);
    AddLongSetting("Grid Snap Size", "Grid", "Snap Size", 100);
}

EditorSettings& EditorSettings::GetInstance() {
    static EditorSettings settings;

    return settings;
}

void EditorSettings::Save() {
    ini.SaveFile(FileSystem::DataPath("Hymn to Beauty", "Settings.ini").c_str());
}
