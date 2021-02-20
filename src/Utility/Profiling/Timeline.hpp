#pragma once

#include <string>
#include <list>
#include <json/json.h>

#include "Thread.hpp"

namespace Profiling {

/// A timeline.
class Timeline {
  public:
    /// Add a thread to the timeline.
    /**
     * @param name The name of the thread to add.
     *
     * @return The thread.
     */
    Thread* AddThread(const std::string& name);

    /// Threads.
    std::list<Thread> threads;

    /// Save timeline to json value.
    /**
     * @return Json representation of the timeline.
     */
    Json::Value ToJson() const;

    /// Load timeline from json value.
    /**
     * @param value The json value to load from.
     */
    void FromJson(const Json::Value& value);

  private:
    void Clear();
};

}
