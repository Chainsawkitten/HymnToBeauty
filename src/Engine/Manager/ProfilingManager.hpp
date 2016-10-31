#pragma once

#include <string>
#include <list>

/// Handles profiling.
class ProfilingManager {
    friend class Hub;
    friend class Profiling;
    
    public:
        
        
    private:
        ProfilingManager();
        ~ProfilingManager();
        ProfilingManager(ProfilingManager const&) = delete;
        void operator=(ProfilingManager const&) = delete;
        
        struct Result {
            std::string name;
            double duration;
            std::list<Result> children;
            Result* parent;
            
            Result(const std::string& name, Result* parent);
        };
        
        Result first = Result("", nullptr);
        Result* current = nullptr;
};
