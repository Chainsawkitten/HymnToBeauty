#pragma once

#include <string>
#include <list>
#include <json/json.h>

#include "Event.hpp"

namespace Profiling {

/// A thread containing events.
class Thread {
  public:
    /// Create new thread.
    /**
     * @param name The name of the new thread.
     */
    explicit Thread(const std::string& name);

    /// Create new event.
    /**
     * @param name The name of the event.
     *
     * @return The event.
     */
    Event* AddEvent(const std::string& name);

    /// Save thread to json value.
    /**
     * @return Json representation of the thread.
     */
    Json::Value ToJson() const;

    /// Load thread from json value.
    /**
     * @param value The json value to load from.
     */
    void FromJson(const Json::Value& value);

    /// The name of the thread.
    std::string name;

    /// Events.
    std::list<Event> events;
};

}
