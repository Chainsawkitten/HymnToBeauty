#include "FiltersWindow.hpp"

#include <Engine/Hymn.hpp>
#include <imgui.h>

using namespace GUI;

void FiltersWindow::Show() {
    // Configure filters.
    if (ImGui::Begin("Filters", &visible)) {
        // Gamma
        if (ImGui::CollapsingHeader("Gamma correction")) {
            ImGui::TextWrapped("Gamma correction filter.");

            ImGui::DragFloat("Gamma", &Hymn().filterSettings.gamma, 0.f, 1.f, 10.f);
        }

        // Color
        if (ImGui::CollapsingHeader("Color")) {
            ImGui::TextWrapped("Color blending filter.");

            ImGui::Checkbox("Enable##Color", &Hymn().filterSettings.colorFilterApply);

            if (Hymn().filterSettings.colorFilterApply) {
                ImGui::ColorEdit3("Color##Color", &Hymn().filterSettings.colorFilterColor[0]);
            }
        }

        // Fog
        if (ImGui::CollapsingHeader("Fog")) {
            ImGui::TextWrapped("Screen-space fog based on the depth buffer.");

            ImGui::Checkbox("Enable##Fog", &Hymn().filterSettings.fogApply);

            if (Hymn().filterSettings.fogApply) {
                ImGui::DragFloat("Density", &Hymn().filterSettings.fogDensity, 0.00001f, 0.f, 0.3f);
                ImGui::ColorEdit3("Color##Fog", &Hymn().filterSettings.fogColor[0]);
            }
        }

        // FXAA
        if (ImGui::CollapsingHeader("FXAA")) {
            ImGui::TextWrapped("Cheap anti-aliasing.");

            ImGui::Checkbox("Enable##FXAA", &Hymn().filterSettings.fxaa);
        }

        // Dither
        if (ImGui::CollapsingHeader("Dither")) {
            ImGui::TextWrapped("Preventing large-scale color patterns such as color banding.");

            ImGui::Checkbox("Enable##Dither", &Hymn().filterSettings.ditherApply);
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
