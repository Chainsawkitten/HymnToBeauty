#pragma once

#include <json/json.h>
#include <glm/glm.hpp>

namespace Json {
    /// Convert a vec3 to a JSON node.
    /**
     * @param value %Value to convert.
     * @return JSON node.
     */
    Json::Value Save(const glm::vec3& value);
}
