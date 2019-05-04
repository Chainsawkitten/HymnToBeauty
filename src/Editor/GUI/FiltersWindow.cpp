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
