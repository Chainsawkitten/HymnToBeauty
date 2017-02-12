#include "Util/Log.hpp"
#include "MainWindow.hpp"
#include "Manager/Managers.hpp"
#include "Manager/RenderManager.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

MainWindow* MainWindow::instance = nullptr;
void WindowSizeCallback(GLFWwindow* window, int width, int height);

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

void MainWindow::Init(bool showNotifications) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);    
    
    if (debugContext)
        glDebugMessageCallback(showNotifications ? DebugMessageCallback : DebugMessageCallbackIgnoreNotifications, nullptr);
}

void MainWindow::SetVsync(bool vsync) {
    glfwSwapInterval(vsync ? 1 : 0);
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

void MainWindow::SetSize(int width, int height){
    size.x = width;
    size.y = height;
}

void MainWindow::SetTitle(const char *title) {
    glfwSetWindowTitle(window, title);
}

bool MainWindow::ShouldClose() const {
    return shouldClose;
}

void MainWindow::Close() {
    shouldClose = true;
}

void MainWindow::SwapBuffers() {
    glfwSwapBuffers(window);
}

GLFWwindow* MainWindow::GetGLFWWindow() const {
    return window;
}

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{

    MainWindow::GetInstance()->SetSize(width, height);
    Managers().renderManager->UpdateBufferSize();

}
