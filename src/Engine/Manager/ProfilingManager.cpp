#include "ProfilingManager.hpp"

#include <imgui.h>

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

void ProfilingManager::ShowResults() {
    ImGui::Begin("Profiling");
    ShowResult(first);
    ImGui::End();
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

void ProfilingManager::ShowResult(Result& result) {
    std::string resultString = result.name + " " + std::to_string(result.duration * 1000.0) + " ms###" + result.name;
    
    if (ImGui::TreeNode(resultString.c_str())) {
        double otherTime = result.duration;
        for (Result& child : result.children) {
            ShowResult(child);
            otherTime -= child.duration;
        }
        
        if (!result.children.empty()) {
            Result other("Other", &result);
            other.duration = otherTime;
            ShowResult(other);
        }
    }
}

ProfilingManager::Result::Result(const std::string& name, Result* parent) {
    this->name = name;
    this->parent = parent;
}
