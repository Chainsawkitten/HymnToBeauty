#include "SettingsWindow.hpp"

#include <imgui.h>
#include "../ImGui/Theme.hpp"

using namespace GUI;

SettingsWindow::SettingsWindow() {
    themes.push_back("Default");
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
        
        ImGui::Combo("Theme", &theme, dropDownItems, themes.size());
        
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
        
        ImGui::ShowStyleEditor();
    }
    
    ImGui::End();
}

bool SettingsWindow::IsVisible() const {
    return visible;
}

void SettingsWindow::SetVisible(bool visible) {
    this->visible = visible;
}
