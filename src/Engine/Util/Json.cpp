#include "Json.hpp"

namespace Json {
    Value Save(const glm::vec3& value) {
        Value node;
        node["x"] = value.x;
        node["y"] = value.y;
        node["z"] = value.z;
        return node;
    }
}
