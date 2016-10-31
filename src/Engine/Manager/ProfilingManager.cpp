#include "ProfilingManager.hpp"

ProfilingManager::ProfilingManager() {
    
}

ProfilingManager::~ProfilingManager() {
    
}

void ProfilingManager::BeginFrame() {
    // Clear previous results.
    first.children.clear();
    first.name = "";
    first.duration = 0.0;
    current = nullptr;
}

ProfilingManager::Result::Result(const std::string& name, Result* parent) {
    this->name = name;
    this->parent = parent;
}
