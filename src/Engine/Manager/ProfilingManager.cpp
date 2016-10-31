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

ProfilingManager::Result* ProfilingManager::StartResult(const std::string& name) {
    if (current == nullptr) {
        first.name = name;
        first.parent = nullptr;
        current = &first;
    } else {
        current->children.push_back(ProfilingManager::Result(name, current));
        Result* result = &current->children.back();
        current = result;
    }
    
    return current;
}

void ProfilingManager::FinishResult(Result* result) {
    current = result->parent;
}

ProfilingManager::Result::Result(const std::string& name, Result* parent) {
    this->name = name;
    this->parent = parent;
}
