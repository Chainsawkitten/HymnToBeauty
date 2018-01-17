#include "MainWindow.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Manager/Managers.hpp"
#include "Manager/RenderManager.hpp"
#include "Util/Input.hpp"

MainWindow* MainWindow::instance = nullptr;
void WindowSizeCallback(GLFWwindow* window, int width, int height);
void ErrorCallback(int error, const char* description);
void HandleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam, bool showNotifications);
void APIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
void APIENTRY DebugMessageCallbackIgnoreNotifications(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

MainWindow::MainWindow(int width, int height, bool fullscreen, bool borderless, const char* title, bool debugContext) {
    
    if (borderless)
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    
    this->debugContext = debugContext;
    if (debugContext)
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    
    GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    
    window = glfwCreateWindow(width, height, title, monitor, nullptr);
    
    if (!window) {
        glfwTerminate();
        /// @todo Print error to log.
    }
    
    glfwMakeContextCurrent(window);
    
    // Setup error callbacks.
    glfwSetErrorCallback(ErrorCallback);
    
    input = new InputHandler(window);
    input->Update();
    input->SetActive();
    
    size = glm::vec2(width, height);
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

void MainWindow::Init(bool showNotifications) const {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);    
    
    if (debugContext)
        glDebugMessageCallback(showNotifications ? DebugMessageCallback : DebugMessageCallbackIgnoreNotifications, nullptr);
}

void MainWindow::Update() {
    input->Update();
    input->SetActive();
    
    if (glfwWindowShouldClose(window) != GLFW_FALSE)
        shouldClose = true;
}

const glm::vec2& MainWindow::GetSize() const {
    return size;
}

void MainWindow::SetSize(int width, int height) {
    size.x = static_cast<float>(width);
    size.y = static_cast<float>(height);
}

void MainWindow::SetTitle(const char *title) const {
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

void MainWindow::SwapBuffers() const {
    glfwSwapBuffers(window);
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
    if (width != 0 && height != 0) {
        Managers().renderManager->UpdateBufferSize();
    }
}

void ErrorCallback(int error, const char* description) {
    fputs(description, stderr);
}

void HandleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam, bool showNotifications) {
    if (!showNotifications && severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;
    
    switch (source) {
    case GL_DEBUG_SOURCE_API:
        fputs("Open GL API", stderr);
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        fputs("Window System", stderr);
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        fputs("Shader Compiler", stderr);
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        fputs("Application", stderr);
        break;
    default:
        fputs("Other", stderr);
    }
    
    fputs(": ", stderr);
    
    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        fputs("Error", stderr);
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        fputs("Deprecated Behavior", stderr);
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        fputs("Undefined Behavior", stderr);
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        fputs("Portability", stderr);
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        fputs("Performance", stderr);
        break;
    case GL_DEBUG_TYPE_MARKER:
        fputs("Marker", stderr);
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        fputs("Push Group", stderr);
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        fputs("Pop Group", stderr);
        break;
    default:
        fputs("Other", stderr);
    }
    
    fputs(" (", stderr);
    
    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        fputs("High Priority", stderr);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        fputs("Medium Priority", stderr);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        fputs("Low Priority", stderr);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        fputs("Notification", stderr);
        break;
    }
    
    fputs("):\n", stderr);
    
    fputs(message, stderr);
    fputs("\n\n", stderr);
    
    fflush(stderr);
}

void APIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    HandleDebugMessage(source, type, id, severity, length, message, userParam, true);
}

void APIENTRY DebugMessageCallbackIgnoreNotifications(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    HandleDebugMessage(source, type, id, severity, length, message, userParam, false);
}
