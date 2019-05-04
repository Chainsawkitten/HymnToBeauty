#include "MainWindow.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Manager/Managers.hpp"
#include "Manager/RenderManager.hpp"
#include "Util/Input.hpp"

MainWindow* MainWindow::instance = nullptr;
void WindowSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);

MainWindow::MainWindow(unsigned int width, unsigned int height, bool fullscreen, bool borderless, const char* title, bool noAPI) {
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

    // Setup error callbacks.
    glfwSetErrorCallback(ErrorCallback);

    input = new InputHandler(window);
    input->Update();
    input->SetActive();

    size = glm::uvec2(width, height);
    instance = this;

    glfwSetWindowSizeCallback(window, WindowSizeCallback);
}

MainWindow::~MainWindow() {
    glfwDestroyWindow(window);
    delete input;
}

MainWindow* MainWindow::GetInstance() {
    return instance;
}

void MainWindow::Update() {
    input->Update();
    input->SetActive();

    if (glfwWindowShouldClose(window) != GLFW_FALSE)
        shouldClose = true;
}

const glm::uvec2& MainWindow::GetSize() const {
    return size;
}

void MainWindow::SetSize(unsigned int width, unsigned int height) {
    size.x = width;
    size.y = height;
}

void MainWindow::SetTitle(const char* title) const {
    glfwSetWindowTitle(window, title);
}

bool MainWindow::ShouldClose() const {
    return shouldClose;
}

void MainWindow::Close() {
    shouldClose = true;
}

void MainWindow::CancelClose() {
    shouldClose = false;
    glfwSetWindowShouldClose(window, GLFW_FALSE);
}

GLFWwindow* MainWindow::GetGLFWWindow() const {
    return window;
}

void MainWindow::SetWindowMode(bool fullscreen, bool borderless) const {
    glfwSetWindowAttrib(window, GLFW_DECORATED, borderless ? GLFW_FALSE : GLFW_TRUE);

    int x, y, width, height;
    glfwGetWindowPos(window, &x, &y);
    glfwGetWindowSize(window, &width, &height);
    glfwSetWindowMonitor(window, fullscreen ? glfwGetPrimaryMonitor() : nullptr, 50, 50, width, height, GLFW_DONT_CARE);
}

void MainWindow::GetWindowMode(bool& fullscreen, bool& borderless) const {
    fullscreen = glfwGetWindowMonitor(window) != nullptr;
    borderless = glfwGetWindowAttrib(window, GLFW_DECORATED) == GLFW_FALSE;
}

void WindowSizeCallback(GLFWwindow* window, int width, int height) {
    MainWindow::GetInstance()->SetSize(width, height);
    if (width > 0 && height > 0) {
        Managers().renderManager->UpdateBufferSize();
    }
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}
