#pragma once

#include <string>
#include <list>

/// Run profiling.
class Profiling {
    public:
        /// Start profiling.
        /**
         * @param name Name of the segment.
         */
        Profiling(const std::string& name);
        
        /// End profiling.
        ~Profiling();
        
    private:
        struct Result {
            std::string name;
            double duration;
            std::list<Result> children;
            Result* parent;
            
            Result(const std::string& name, Result* parent);
        };
        
        Result* result;
        double start;
};

#define PROFILE(name) Profiling __profileInstance(name)
