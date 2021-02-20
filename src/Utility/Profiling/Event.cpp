#include "Event.hpp"

namespace Profiling {

Event::Event(const std::string& name) {
    this->name = name;
}

Json::Value Event::ToJson() const {
    Json::Value value;

    value["name"] = name;
    value["time"] = time;
    value["duration"] = duration;

    Json::Value childrenJson;
    for (const Event& event : children) {
        childrenJson.append(event.ToJson());
    }

    value["children"] = childrenJson;

    return value;
}

void Event::FromJson(const Json::Value& value) {
    FromJson(value, nullptr);
}

void Event::FromJson(const Json::Value& value, Event* parent) {
    this->parent = parent;
    name = value["name"].asString();
    time = value["time"].asDouble();
    duration = value["duration"].asDouble();

    const Json::Value& childrenJson = value["children"];
    for (Json::ArrayIndex i = 0; i < childrenJson.size(); i++) {
        children.push_back(Event(""));
        Event* event = &children.back();
        event->FromJson(childrenJson[i], this);
    }
}

}
