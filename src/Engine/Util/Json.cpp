#include "Json.hpp"

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
    
    Value SaveImVec2(const ImVec2& value) {
        Value node;
        node["x"] = value.x;
        node["y"] = value.y;
        return node;
    }
    
    ImVec2 LoadImVec2(const Value& value) {
        return ImVec2(value.get("x", 0.f).asFloat(), value.get("y", 0.f).asFloat());
    }
    
    Value SaveImVec4(const ImVec4& value) {
        Value node;
        node["x"] = value.x;
        node["y"] = value.y;
        node["z"] = value.z;
        node["w"] = value.w;
        return node;
    }
    
    ImVec4 LoadImVec4(const Value& value) {
        return ImVec4(value.get("x", 0.f).asFloat(), value.get("y", 0.f).asFloat(), value.get("z", 0.f).asFloat(), value.get("w", 0.f).asFloat());
    }
}
