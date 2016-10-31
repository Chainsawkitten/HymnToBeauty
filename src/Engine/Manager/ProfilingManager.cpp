#include "ProfilingManager.hpp"

ProfilingManager::ProfilingManager() {
    
}

ProfilingManager::~ProfilingManager() {
    
}

ProfilingManager::Result::Result(const std::string& name, Result* parent) {
    this->name = name;
    this->parent = parent;
}
