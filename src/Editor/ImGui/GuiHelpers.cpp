#include "GuiHelpers.hpp"

namespace ImGui {
void ShowHelpMarker(const char* description, const float sameLineOffset) {
    if (sameLineOffset > 0.0f)
        ImGui::SameLine(sameLineOffset);

    ImGui::TextDisabled("[?]");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(450.0f);
        ImGui::TextUnformatted(description);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void DraggableFloat(const char* label, float& value, float min, float max) {
    ImGui::DragFloat(label, &value, 0.001f * std::abs(ImGui::GetIO().MouseDelta.x));

    if (ImGui::GetMouseCursor() == 0 && (ImGui::IsItemHovered() || (ImGui::IsItemActive() && ImGui::GetIO().MouseDown)))
        ImGui::SetMouseCursor(4);

    if (value < min)
        value = min;

    if (value > max)
        value = max;
}

void DraggableVec2(const char* label, glm::vec2& value, float min, float max) {
    ImGui::DragFloat2(label, &value[0], 0.001f * std::abs(ImGui::GetIO().MouseDelta.x));

    if (ImGui::GetMouseCursor() == 0 && (ImGui::IsItemHovered() || (ImGui::IsItemActive() && ImGui::GetIO().MouseDown)))
        ImGui::SetMouseCursor(4);

    if (value.x < min)
        value.x = min;
    if (value.y < min)
        value.y = min;

    if (value.x > max)
        value.x = max;
    if (value.y > max)
        value.y = max;
}

void DraggableVec3(const char* label, glm::vec3& value, float min, float max) {
    ImGui::DragFloat3(label, &value[0], 0.001f * std::abs(ImGui::GetIO().MouseDelta.x));

    if (ImGui::GetMouseCursor() == 0 && (ImGui::IsItemHovered() || (ImGui::IsItemActive() && ImGui::GetIO().MouseDown)))
        ImGui::SetMouseCursor(4);

    if (value.x < min)
        value.x = min;
    if (value.y < min)
        value.y = min;
    if (value.z < min)
        value.z = min;

    if (value.x > max)
        value.x = max;
    if (value.y > max)
        value.y = max;
    if (value.z > max)
        value.z = max;
}
} // namespace ImGui
