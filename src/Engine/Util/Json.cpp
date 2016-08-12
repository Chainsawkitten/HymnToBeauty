#include "Json.hpp"

namespace Json {
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
}
