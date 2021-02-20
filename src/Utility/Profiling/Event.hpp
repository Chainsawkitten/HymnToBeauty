#pragma once

#include <string>
#include <list>
#include <json/json.h>

namespace Profiling {

/// A profiling event on the timeline.
class Event {
  public:
    /// Create new event.
    /**
     * @param name The name of the event.
     */
    explicit Event(const std::string& name);

    /// Save event to json value.
    /**
     * @return Json representation of the event.
     */
    Json::Value ToJson() const;

    /// Load event from json value.
    /**
     * @param value The json value to load from.
     */
    void FromJson(const Json::Value& value);

    /// The name of the event.
    std::string name;

    /// The time the event occured.
    double time;

    /// The duration of the event.
    double duration;

    /// Child events.
    std::list<Event> children;

    /// Parent event.
    Event* parent = nullptr;

  private:
      void FromJson(const Json::Value& value, Event* parent);
};

}
