#include "Splitter.hpp"

#include <imgui.h>

namespace ImGui {
    void VerticalSplitter(const ImVec2& position, int width, int splitterSize, int& value, bool& active, int min, int max) {
        if (ImGui::GetIO().MousePos.x > position.x && ImGui::GetIO().MousePos.x < position.x + width &&
            ImGui::GetIO().MousePos.y >= position.y - splitterSize && ImGui::GetIO().MousePos.y <= position.y + splitterSize) {
            ImGui::SetMouseCursor(3);
            
            if (ImGui::GetIO().MouseClicked[0])
                active = true;
        }
        
        if (!ImGui::GetIO().MouseDown[0])
            active = false;
        
        if (active) {
            ImGui::SetMouseCursor(3);
            value = ImGui::GetIO().MousePos.y;
        }
        
        if (value < min)
            value = min;
        
        if (value > max)
            value = max;
    }
}
