#pragma once

#include <json/json.h>
#include <glm/glm.hpp>

namespace Json {
    /// Convert a vec2 to a JSON node.
    /**
     * @param value %Value to convert.
     * @return JSON node.
     */
    Json::Value SaveVec2(const glm::vec2& value);

    /// Load a vec2 from a JSON node.
    /**
     * @param value The JSON node to load from.
     * @return The loaded vec2.
     */
    glm::vec2 LoadVec2(const Value& value);

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
    glm::vec3 LoadVec3(const Value& value);

    /// Convert a quaternion into a JSON node.
    /**
     * @param value %Value to convert.
     * @return JSON node.
     */
    Json::Value SaveQuaternion(const glm::quat& value);

    /// Load a quaternion from a JSON node.
    /**
     * @param value The JSON node to load from.
     * @return The loaded quaternion
     */
    glm::quat LoadQuaternion(const Value& value);
}
