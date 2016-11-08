#include "Profiling.hpp"

#include <GLFW/glfw3.h>
#include "../Manager/Managers.hpp"

using namespace std;

Profiling::Profiling(const std::string& name) {
    result = Managers().profilingManager->StartResult(name);
    start = glfwGetTime();
}

Profiling::~Profiling() {
    Managers().profilingManager->FinishResult(result, start);
}
