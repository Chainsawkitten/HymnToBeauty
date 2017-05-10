#include "Draggable.hpp"

#include <imgui.h>
#include <cmath>

namespace ImGui {
    void DraggableFloat(const char* label, float& value, float min, float max) {
        ImGui::DragFloat(label, &value, 0.001f*std::abs(ImGui::GetIO().MouseDelta.x));
        
        if (value < min)
            value = min;
        
        if (value > max)
            value = max;
    }
}
