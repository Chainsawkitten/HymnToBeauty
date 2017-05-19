#include "SettingsWindow.hpp"

#include <imgui.h>

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
