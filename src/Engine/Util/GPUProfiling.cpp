#include "GPUProfiling.hpp"

#include <assert.h>
#include "Engine/Manager/Managers.hpp"
#include <Utility/Log.hpp>

GPUProfiling::GPUProfiling(const std::string& name, Video::Query::Type type) : active(false) {
    // Check if profiling.
    if (Managers().profilingManager->Active()) {
        active = true;

        // Get type.
        switch (type) {
            case Video::Query::TIME_ELAPSED:
                this->type = ProfilingManager::Type::GPU_TIME_ELAPSED;
                break;
            case Video::Query::SAMPLES_PASSED:
                this->type = ProfilingManager::Type::GPU_SAMPLES_PASSED;
                break;
            default:
                assert(false);
                break;
        }

        // Check if nested.
        if (this->type == ProfilingManager::Type::GPU_SAMPLES_PASSED) {
            if (Managers().profilingManager->current[this->type] != Managers().profilingManager->root[this->type] &&
                Managers().profilingManager->current[this->type]->parent == Managers().profilingManager->root[this->type]) {
                Log() << "Warning: GPU_SAMPLES_PASSED can't be nested! Name: " << name << "\n";
                active = false;
            }
        }
    }
    // Start profling if active.
    if (active)
        result = Managers().profilingManager->StartResult(name, this->type);
}

GPUProfiling::~GPUProfiling() {
    if (active)
        Managers().profilingManager->FinishResult(result, type);
}
