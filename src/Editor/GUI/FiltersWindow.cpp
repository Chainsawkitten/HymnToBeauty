#include "FiltersWindow.hpp"

#include <Engine/Hymn.hpp>
#include <imgui.h>

using namespace GUI;

void FiltersWindow::Show() {
    // Configure filters.
    if (ImGui::Begin("Filters", &visible)) {
        // Bloom
        if (ImGui::CollapsingHeader("Bloom")) {
            ImGui::TextWrapped("Causes bright areas of the image to bleed.");

            ImGui::Checkbox("Enable##Bloom", &Hymn().filterSettings.bloom);
            ImGui::DragFloat("Intensity", &Hymn().filterSettings.bloomIntensity, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Threshold", &Hymn().filterSettings.bloomThreshold, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Scatter", &Hymn().filterSettings.bloomScatter, 0.01f, 0.0f, 1.0f);
        }

        // Gamma
        if (ImGui::CollapsingHeader("Gamma correction")) {
            ImGui::TextWrapped("Gamma correction filter.");

            ImGui::DragFloat("Gamma", &Hymn().filterSettings.gamma, 0.1f, 1.0f, 10.0f);
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
