#pragma once

namespace ImGui {
    /// Show widget to edit a float.
    /**
     * @param label Label describing the value being edited.
     * @param value Value to edit.
     */
    void DraggableFloat(const char* label, float* value);
}
