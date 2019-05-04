#include "ProfilingManager.hpp"

#include <GLFW/glfw3.h>
#include <Utility/Log.hpp>

ProfilingManager::ProfilingManager() : active(false) {
    for (int i = 0; i < Type::COUNT; ++i) {
        root[i] = new Result("Root: " + TypeToString((Type)i), nullptr);
        root[i]->parent = nullptr;
    }
}

ProfilingManager::~ProfilingManager() {
    for (int i = 0; i < Type::COUNT; ++i) {
        delete root[i];
    }
}

void ProfilingManager::BeginFrame() {
    if (!active) {
        Log() << "ProfilingManager::BeginFrame warning: Not active.\n";
        return;
    }

    // Clear previous results.
    for (int i = 0; i < Type::COUNT; ++i) {
        root[i]->children.clear();
        current[i] = root[i];
    }
    frameStart = glfwGetTime();
}

void ProfilingManager::EndFrame() {
    // Calculate the CPU time of this frame.
    frameTimes[0][frame] = static_cast<float>((glfwGetTime() - frameStart) * 1000.0);

    if (++frame >= frames)
        frame = 0;
}

bool ProfilingManager::Active() const {
    return active;
}

void ProfilingManager::SetActive(bool active) {
    this->active = active;
}

unsigned int ProfilingManager::GetFrameCount() const {
    return frames;
}

const float* ProfilingManager::GetCPUFrameTimes() const {
    return frameTimes[0];
}

ProfilingManager::Result* ProfilingManager::GetResult(Type type) const {
    return root[type];
}

std::string ProfilingManager::TypeToString(ProfilingManager::Type type) {
    switch (type) {
    case ProfilingManager::CPU_TIME:
        return "CPU time (ms)";
        break;
    default:
        assert(false);
        return "ProfilingWindow::TypeToString warning: No valid type to string";
        break;
    }
}

ProfilingManager::Result* ProfilingManager::StartResult(const std::string& name, Type type) {
    assert(active);
    assert(type != COUNT);

    current[type]->children.push_back(ProfilingManager::Result(name, current[type]));
    Result* result = &current[type]->children.back();
    current[type] = result;

    return result;
}

void ProfilingManager::FinishResult(Result* result, Type type) {
    assert(active);
    assert(type != COUNT);
    assert(result == current[type]);

    current[type] = result->parent;
}

ProfilingManager::Result::Result(const std::string& name, Result* parent) : name(name) {
    this->parent = parent;
}
