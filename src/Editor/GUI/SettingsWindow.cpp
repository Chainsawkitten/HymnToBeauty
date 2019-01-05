#include "SettingsWindow.hpp"

#include <imgui.h>
#include "../ImGui/Theme.hpp"
#include <Engine/Util/FileSystem.hpp>
#include "../Util/EditorSettings.hpp"

using namespace GUI;

SettingsWindow::SettingsWindow() {
    themeName[0] = '\0';
    themes.push_back("Default");

    // Fetch a list of all themes (JSON files in Themes directory).
    std::vector<std::string> themeFiles = FileSystem::DirectoryContents(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Themes", FileSystem::FILE);
    for (std::size_t i = 0; i < themeFiles.size(); ++i) {
        if (FileSystem::GetExtension(themeFiles[i]) == "json") {
            std::string name = themeFiles[i].substr(0, themeFiles[i].find_last_of("."));
            themes.push_back(name);
            if (name == EditorSettings::GetInstance().GetString("Theme"))
                theme = static_cast<int>(i) + 1;
        }
    }
}

void SettingsWindow::Show() {
    if (ImGui::Begin("Settings", &visible)) {
        // Show drop down list to select theme.
        if (dropDownItems != nullptr)
            delete[] dropDownItems;
        dropDownItems = new const char*[themes.size()];
        for (std::size_t i = 0; i < themes.size(); ++i) {
            dropDownItems[i] = themes[i].c_str();
        }

        int previousTheme = theme;
        ImGui::Combo("Theme", &theme, dropDownItems, static_cast<int>(themes.size()));

        // If a different theme was selected, load it.
        if (theme != previousTheme) {
            if (theme == 0)
                ImGui::LoadDefaultTheme();
            else
                ImGui::LoadTheme(themes[theme].c_str());

            EditorSettings::GetInstance().SetString("Theme", themes[theme]);
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
