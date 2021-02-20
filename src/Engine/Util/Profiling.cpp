#include "Profiling.hpp"

#include "../Manager/Managers.hpp"

ProfilingScope::ProfilingScope(const std::string& name) {
    if (Managers().profilingManager->Active()) {
        event = Managers().profilingManager->StartEvent(name);
    }
}

ProfilingScope::~ProfilingScope() {
    if (Managers().profilingManager->Active()) {
        Managers().profilingManager->FinishEvent(event);
    }
}
