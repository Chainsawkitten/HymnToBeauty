#pragma once
#include <imgui.h>

namespace ImGui {
    /// Helper function that inserts a help marker that displays a tooltip.
    /**
     * @param description The content of the tooltip.
     * @param sameLineOffset If set to higher than 0.f, the help marker will be offset on the same line.
     */
    static void ShowHelpMarker(const char* description, const float sameLineOffset = 0.0f)
    {
        if(sameLineOffset > 0.0f)
            ImGui::SameLine(sameLineOffset);
        
        ImGui::TextDisabled("[?]");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(450.0f);
            ImGui::TextUnformatted(description);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }
}
