#include "SettingsWindow.hpp"

#include <imgui.h>

using namespace GUI;

void SettingsWindow::Show() {
    if (ImGui::Begin("Settings", &visible)) {
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
