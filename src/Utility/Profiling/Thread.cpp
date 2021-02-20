#include "Thread.hpp"

namespace Profiling {

Thread::Thread(const std::string& name) {
    this->name = name;
}

Event* Thread::AddEvent(const std::string& name) {
    events.push_back(Event(name));
    return &events.back();
}

Json::Value Thread::ToJson() const {
    Json::Value value;

    value["name"] = name;

    Json::Value eventsJson;
    for (const Event& event : events) {
        eventsJson.append(event.ToJson());
    }

    value["events"] = eventsJson;

    return value;
}

void Thread::FromJson(const Json::Value& value) {
    name = value["name"].asString();

    const Json::Value& eventsJson = value["events"];
    for (Json::ArrayIndex i = 0; i < eventsJson.size(); i++) {
        Event* event = AddEvent("");
        event->FromJson(eventsJson[i]);
    }
}

}
