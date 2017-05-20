#include "GuiHelpers.hpp"

void ImGui::ShowHelpMarker(const char* description, const float sameLineOffset) {
    if(sameLineOffset > 0.0f)
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
