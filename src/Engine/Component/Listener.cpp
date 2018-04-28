#include "Listener.hpp"

using namespace Component;

Listener::Listener() {
}

Json::Value Listener::Save() const {
    Json::Value component;
    component["placeholderValue"] = "";
    return component;
}
