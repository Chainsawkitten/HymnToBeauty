#include "Json.hpp"
#include <glm/gtc/quaternion.hpp>

namespace Json {
Value SaveVec2(const glm::vec2& value) {
    Value node;
    node["x"] = value.x;
    node["y"] = value.y;
    return node;
}

glm::vec2 LoadVec2(const Value& value) {
    return glm::vec2(value.get("x", 0.f).asFloat(), value.get("y", 0.f).asFloat());
}

Value SaveVec3(const glm::vec3& value) {
    Value node;
    node["x"] = value.x;
    node["y"] = value.y;
    node["z"] = value.z;
    return node;
}

glm::vec3 LoadVec3(const Value& value) {
    return glm::vec3(value.get("x", 0.f).asFloat(), value.get("y", 0.f).asFloat(), value.get("z", 0.f).asFloat());
}

Value SaveQuaternion(const glm::quat& value) {
    Value node;
    node["w"] = value.w;
    node["x"] = value.x;
    node["y"] = value.y;
    node["z"] = value.z;
    return node;
}

glm::quat LoadQuaternion(const Value& value) {
    return glm::quat(value.get("w", 0.0f).asFloat(), value.get("x", 0.0f).asFloat(), value.get("y", 0.0f).asFloat(), value.get("z", 0.0f).asFloat());
}
} // namespace Json
