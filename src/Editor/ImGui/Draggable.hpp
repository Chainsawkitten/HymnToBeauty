#pragma once

#include <limits>
#include <glm/glm.hpp>

namespace ImGui {
    /// Show widget to edit a float.
    /**
     * @param label Label describing the value being edited.
     * @param value Value to edit.
     * @param min Minimum allowed value.
     * @param max Maximum allowed value.
     */
    void DraggableFloat(const char* label, float& value, float min = std::numeric_limits<float>::lowest(), float max = (std::numeric_limits<float>::max)());
    
    /// Show widget to edit a vec2.
    /**
     * @param label Label describing the value being edited.
     * @param value Value to edit.
     * @param min Minimum allowed value.
     * @param max Maximum allowed value.
     */
    void DraggableVec2(const char* label, glm::vec2& value, float min = std::numeric_limits<float>::lowest(), float max = (std::numeric_limits<float>::max)());
    
    /// Show widget to edit a vec3.
    /**
     * @param label Label describing the value being edited.
     * @param value Value to edit.
     * @param min Minimum allowed value.
     * @param max Maximum allowed value.
     */
    void DraggableVec3(const char* label, glm::vec3& value, float min = std::numeric_limits<float>::lowest(), float max = (std::numeric_limits<float>::max)());
}
