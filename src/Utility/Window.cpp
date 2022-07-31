#include "Window.hpp"

#include <cassert>
#include <cstdio>
#include <map>
#include "Log.hpp"

#if ANDROID
#else
#include <GLFW/glfw3.h>
#endif

namespace Utility {

#if ANDROID
Window::Window(ANativeWindow* androidWindow) {
    window = androidWindow;
    size = glm::uvec2(ANativeWindow_getWidth(androidWindow), ANativeWindow_getHeight(androidWindow));
}
#else
static std::map<GLFWwindow*, Window*> sizeCallbackMap;

static void WindowSizeCallback(GLFWwindow* window, int width, int height) {
    sizeCallbackMap[window]->SetSize(glm::uvec2(width, height));
}

static void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

Window::Window(unsigned int width, unsigned int height, bool fullscreen, bool borderless, const char* title, bool noAPI) {
    assert(width > 0 && height > 0);
    assert(title != nullptr);

    if (borderless)
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    if (noAPI)
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#ifndef NDEBUG
    if (!noAPI)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    window = glfwCreateWindow(width, height, title, monitor, nullptr);

    if (!window) {
        glfwTerminate();
        /// @todo Print error to log.
    }

    // Setup callbacks.
    glfwSetErrorCallback(ErrorCallback);
    glfwSetWindowSizeCallback(window, WindowSizeCallback);
    sizeCallbackMap[window] = this;
    
    size = glm::uvec2(width, height);
}
#endif

Window::~Window() {
#if ANDROID
#else
    glfwDestroyWindow(window);
#endif
}

void Window::SetSize(const glm::uvec2& size) {
    this->size = size;
}

const glm::uvec2& Window::GetSize() const {
    return size;
}

void Window::Update() {
#if !ANDROID
    if (glfwWindowShouldClose(window) != GLFW_FALSE)
        shouldClose = true;
#endif
}

bool Window::ShouldClose() const {
    return shouldClose;
}

void Window::CancelClose() {
#if ANDROID
    Log(Log::ERR) << "Window::CancelClose should not be called on Android.";
#else
    shouldClose = false;
    glfwSetWindowShouldClose(window, GLFW_FALSE);
#endif
}

void Window::SetWindowMode(bool fullscreen, bool borderless) const {
#if ANDROID
    Log(Log::ERR) << "SetWindowMode not implemented on Android.";
#else
    glfwSetWindowAttrib(window, GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);

    int x, y, width, height;
    glfwGetWindowPos(window, &x, &y);
    glfwGetWindowSize(window, &width, &height);
    glfwSetWindowMonitor(window, fullscreen ? glfwGetPrimaryMonitor() : nullptr, 50, 50, width, height, GLFW_DONT_CARE);
#endif
}

void Window::GetWindowMode(bool& fullscreen, bool& borderless) const {
#if ANDROID
    fullscreen = true;
    borderless = false;
#else
    fullscreen = glfwGetWindowMonitor(window) != nullptr;
    borderless = glfwGetWindowAttrib(window, GLFW_DECORATED) == GLFW_FALSE;
#endif
}

#if ANDROID
ANativeWindow* Window::GetAndroidWindow() {
    return window;
}
#else
GLFWwindow* Window::GetGLFWWindow() {
    return window;
}
#endif

}
