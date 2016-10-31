#include "Profiling.hpp"

#include <GLFW/glfw3.h>

using namespace std;

Profiling::Profiling(const std::string& name) {
    /*if (Profiling::current == nullptr) {
        first.name = name;
        first.parent = nullptr;
        current = &first;
        result = &first;
    } else {
        current->children.push_back(Result(name, current));
        result = &current->children.back();
        current = result;
    }*/
    
    start = glfwGetTime();
}

Profiling::~Profiling() {
    /*result->duration = glfwGetTime() - start;
    if (current == result)
        current = result->parent;*/
}

Profiling::Result::Result(const std::string& name, Result* parent) {
    this->name = name;
    this->parent = parent;
}
