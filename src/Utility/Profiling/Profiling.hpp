#pragma once

#include <string>

namespace Profiling {

class Event;

/// Whether CPU profiling is active.
extern bool cpuActive;

/// The current profiling event.
extern Event* currentEvent;

/// Run profiling.
class ProfilingScope {
  public:
    /// Start profiling.
    /**
     * @param name Name of the segment.
     */
    explicit ProfilingScope(const std::string& name);

    /// End profiling.
    ~ProfilingScope();

  private:
    Event* event;
    double start;
};

}

#define PROFILE(name) Profiling::ProfilingScope __profileInstance(name)
