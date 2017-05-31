#include "FiltersWindow.hpp"

#include <Engine/Hymn.hpp>
#include <imgui.h>

using namespace GUI;

void FiltersWindow::Show() {
    // Configure filters.
    if (ImGui::Begin("Filters", &visible, ImGuiWindowFlags_ShowBorders)) {
        // Fog
        if (ImGui::CollapsingHeader("Fog")) {
            ImGui::Checkbox("Enable##Fog", &Hymn().filterSettings.fog);
            
            if (Hymn().filterSettings.fog) {
                ImGui::InputFloat("Density", &Hymn().filterSettings.fogDensity);
                ImGui::ColorEdit3("Color", &Hymn().filterSettings.fogColor[0]);
            }
        }
        
        // FXAA
        if (ImGui::CollapsingHeader("FXAA")) {
            ImGui::Checkbox("Enable##FXAA", &Hymn().filterSettings.fxaa);
        }
        
        // Glow
        if (ImGui::CollapsingHeader("Glow")) {
            ImGui::Checkbox("Enable##Glow", &Hymn().filterSettings.glow);
            
            if (Hymn().filterSettings.glow) {
                ImGui::InputInt("Blur amount", &Hymn().filterSettings.glowBlurAmount);
            }
        }
    }
    ImGui::End();
}

bool FiltersWindow::IsVisible() const {
    return visible;
}

void FiltersWindow::SetVisible(bool visible) {
    this->visible = visible;
}
