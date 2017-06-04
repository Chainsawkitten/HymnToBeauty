#include "FiltersWindow.hpp"

#include <Engine/Hymn.hpp>
#include <imgui.h>

using namespace GUI;

void FiltersWindow::Show() {
    // Configure filters.
    if (ImGui::Begin("Filters", &visible, ImGuiWindowFlags_ShowBorders)) {
        // Color
        if (ImGui::CollapsingHeader("Color")) {
            ImGui::TextWrapped("Color blending filter.");
            
            ImGui::Checkbox("Enable##Color", &Hymn().filterSettings.color);
            
            if (Hymn().filterSettings.color) {
                ImGui::ColorEdit3("Color##Color", &Hymn().filterSettings.colorColor[0]);
            }
        }
        
        // Fog
        if (ImGui::CollapsingHeader("Fog")) {
            ImGui::TextWrapped("Screen-space fog based on the depth buffer.");
            
            ImGui::Checkbox("Enable##Fog", &Hymn().filterSettings.fog);
            
            if (Hymn().filterSettings.fog) {
                ImGui::InputFloat("Density", &Hymn().filterSettings.fogDensity);
                ImGui::ColorEdit3("Color##Fog", &Hymn().filterSettings.fogColor[0]);
            }
        }
        
        // FXAA
        if (ImGui::CollapsingHeader("FXAA")) {
            ImGui::TextWrapped("Cheap anti-aliasing.");
            
            ImGui::Checkbox("Enable##FXAA", &Hymn().filterSettings.fxaa);
        }
        
        // Glow
        if (ImGui::CollapsingHeader("Glow")) {
            ImGui::TextWrapped("Blur the glow buffer and render it using additive blending.");
            
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
