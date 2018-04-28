#pragma once

#include <string>
#include "../Manager/ProfilingManager.hpp"

/// Run profiling.
class Profiling {
    public:
        /// Start profiling.
        /**
         * @param name Name of the segment.
         */
        explicit Profiling(const std::string& name);

        /// End profiling.
        ~Profiling();

    private:
        ProfilingManager::Result* result;
        double start;
};

#define PROFILE(name) Profiling __profileInstance(name)
