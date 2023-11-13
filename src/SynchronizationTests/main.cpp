#include "Framework/Runner.hpp"
#include "Tests/FragFragImage.hpp"
#include "Tests/FragFragBuffer.hpp"
#include "Tests/FragVertImage.hpp"
#include "Tests/ComputeFragBuffer.hpp"
#include "Tests/ComputeVertBuffer.hpp"
#include "Tests/MultipleFrames.hpp"
#include "Tests/BarrierMerging.hpp"
#include "Tests/PrimitiveRestart.hpp"

#include <Utility/Log.hpp>
#include <Video/Renderer.hpp>
#include <Utility/Window.hpp>
#include <GLFW/glfw3.h>

int main(int argc, char* argv[]) {
    Video::Renderer::GraphicsAPI graphicsAPI = Video::Renderer::GraphicsAPI::WEBGPU;

    glfwInit();
    Utility::Window* window = new Utility::Window(800, 600, false, false, "Synchronization tests",
        graphicsAPI == Video::Renderer::GraphicsAPI::VULKAN || graphicsAPI == Video::Renderer::GraphicsAPI::WEBGPU);

    PrimitiveRestart test;
    RunTest(&test, window, graphicsAPI);

    delete window;
    glfwTerminate();

    return 0;
}
