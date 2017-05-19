#include "SettingsWindow.hpp"

#include <imgui.h>
#include "../ImGui/Theme.hpp"
#include <Engine/Util/FileSystem.hpp>

using namespace GUI;

SettingsWindow::SettingsWindow() {
    themes.push_back("Default");
    
    std::vector<std::string> themeFiles = FileSystem::DirectoryContents(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Themes", FileSystem::FILE);
    for (std::string theme : themeFiles) {
        if (FileSystem::GetExtension(theme) == "json")
            themes.push_back(theme.substr(0, theme.find_last_of(".")));
    }
}

void SettingsWindow::Show() {
    if (ImGui::Begin("Settings", &visible)) {
        // Show drop down list to select theme.
        if (dropDownItems != nullptr)
            delete[] dropDownItems;
        dropDownItems = new const char*[themes.size()];
        for (std::size_t i=0; i < themes.size(); ++i) {
            dropDownItems[i] = themes[i].c_str();
        }
        
        int previousTheme = theme;
        ImGui::Combo("Theme", &theme, dropDownItems, themes.size());
        
        // If a different theme was selected, load it.
        if (theme != previousTheme) {
            if (theme == 0)
                ImGui::LoadDefaultTheme();
            else
                ImGui::LoadTheme(themes[theme].c_str());
        }
        
        // Clone current theme to create a new theme.
        if (ImGui::Button("Clone")) {
            ImGui::OpenPopup("Theme Name");
            themeName[0] = '\0';
        }
        
        if (ImGui::BeginPopupModal("Theme Name", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::InputText("Name", themeName, 128);
            
            if (ImGui::Button("Create")) {
                ImGui::SaveTheme(themeName);
                themes.push_back(themeName);
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
                ImGui::CloseCurrentPopup();
            
            ImGui::EndPopup();
        }
        
        if (theme != 0) {
            ImGui::SameLine();
            if (ImGui::Button("Save"))
                ImGui::SaveTheme(themes[theme].c_str());
            
            ImGui::Separator();
            
            // Edit the current theme.
            ImGui::ShowStyleEditor();
        }
    }
    
    ImGui::End();
}

bool SettingsWindow::IsVisible() const {
    return visible;
}

void SettingsWindow::SetVisible(bool visible) {
    this->visible = visible;
}
