#include "Framework/Runner.hpp"
#include "Tests/ExampleTest.hpp"

#include <Utility/Log.hpp>
#include <Video/Renderer.hpp>
#include <Utility/Window.hpp>
#include <GLFW/glfw3.h>

int main(int argc, char* argv[]) {
    Video::Renderer::GraphicsAPI graphicsAPI = Video::Renderer::GraphicsAPI::VULKAN;

    glfwInit();
    Utility::Window* window = new Utility::Window(800, 600, false, false, "Synchronization tests",
        graphicsAPI == Video::Renderer::GraphicsAPI::VULKAN || graphicsAPI == Video::Renderer::GraphicsAPI::WEBGPU);

    ExampleTest test;
    RunTest(&test, window, graphicsAPI);

    delete window;
    glfwTerminate();

    return 0;
}
