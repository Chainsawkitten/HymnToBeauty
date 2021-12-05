#include "Profiling.hpp"

#include "Event.hpp"
#include <GLFW/glfw3.h>
#include <cassert>

namespace Profiling {

bool cpuActive = false;
Event* currentEvent = nullptr;

static Event* StartEvent(const std::string& name) {
    assert(cpuActive);

    currentEvent->children.push_back(Profiling::Event(name));
    Profiling::Event* event = &currentEvent->children.back();
    event->parent = currentEvent;
    event->time = glfwGetTime();
    currentEvent = event;

    return event;
}

static void FinishEvent(Profiling::Event* event) {
    assert(cpuActive);
    assert(event != nullptr);

    currentEvent->duration = glfwGetTime() - currentEvent->time;
    currentEvent = currentEvent->parent;
}

ProfilingScope::ProfilingScope(const std::string& name) {
    if (cpuActive) {
        event = StartEvent(name);
    }
}

ProfilingScope::~ProfilingScope() {
    if (cpuActive) {
        FinishEvent(event);
    }
}

}
