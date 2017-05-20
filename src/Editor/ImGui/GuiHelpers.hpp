#pragma once
#include <imgui.h>

namespace ImGui {
    /// Helper function that inserts a help marker that displays a tooltip.
    /**
     * @param description The content of the tooltip.
     * @param sameLineOffset If set to higher than 0.f, the help marker will be offset on the same line.
     */
    void ShowHelpMarker(const char* description, const float sameLineOffset = 0.0f);
}
