#pragma once

#include <json/json.h>
#include <glm/glm.hpp>

namespace Json {
    /// Convert a vec3 to a JSON node.
    /**
     * @param value %Value to convert.
     * @return JSON node.
     */
    Json::Value SaveVec3(const glm::vec3& value);
    
    /// Load a vec3 from a JSON node.
    /**
     * @param value The JSON node to load from.
     * @return The loaded vec3.
     */
    glm::vec3 LoadVec3(const Json::Value& value);
}
