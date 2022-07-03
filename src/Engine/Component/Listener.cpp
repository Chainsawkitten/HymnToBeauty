#include "Listener.hpp"

#include "../Util/Json.hpp"

using namespace Component;

Listener::Listener() {}

void Listener::Serialize(Json::Value& node, bool load) {
    int32_t placeholderValue = 0;
    Json::Serialize(node, load, "placeholderValue", placeholderValue, 0);
}
