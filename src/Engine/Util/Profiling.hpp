#pragma once

#include <string>
#include "../Manager/ProfilingManager.hpp"

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
    Profiling::Event* event;
    double start;
};

#define PROFILE(name) ProfilingScope __profileInstance(name)
