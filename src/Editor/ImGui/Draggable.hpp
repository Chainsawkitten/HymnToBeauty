#pragma once

#include <limits>

namespace ImGui {
    /// Show widget to edit a float.
    /**
     * @param label Label describing the value being edited.
     * @param value Value to edit.
     * @param min Minimum allowed value.
     * @param max Maximum allowed value.
     */
    void DraggableFloat(const char* label, float& value, float min = std::numeric_limits<float>::lowest(), float max = (std::numeric_limits<float>::max)());
}
