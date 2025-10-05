#include "Runner.hpp"

#include "SynchronizationTest.hpp"

#if OPENGL_SUPPORT
#include <Video/LowLevelRenderer/OpenGL/OpenGLRenderer.hpp>
#endif
#if VULKAN_SUPPORT
#include <Video/LowLevelRenderer/Vulkan/VulkanRenderer.hpp>
#endif
#if WEBGPU_SUPPORT
#include <Video/LowLevelRenderer/WebGPU/WebGPURenderer.hpp>
#endif
#include <Utility/Log.hpp>
#include <Utility/Time.hpp>
#include <Utility/Window.hpp>
#if !__ANDROID__
#include <GLFW/glfw3.h>
#endif

static SynchronizationTest* theTest;
static Utility::Window* theWindow;
static Video::LowLevelRenderer* renderer = nullptr;

void RunTest(SynchronizationTest* test, Utility::Window* window, Video::Renderer::GraphicsAPI graphicsAPI) {
    SetupTest(test, window, graphicsAPI);

    // Main loop.
    while (!theWindow->ShouldClose()) {
        theWindow->Update();
        RunFrame();
    }

    ShutdownTest();
}

void SetupTest(SynchronizationTest* test, Utility::Window* window, Video::Renderer::GraphicsAPI graphicsAPI) {
    theTest = test;
    theWindow = window;

    Utility::InitTime();

    switch (graphicsAPI) {
#if OPENGL_SUPPORT
    case Video::Renderer::GraphicsAPI::OPENGL:
        renderer = new Video::OpenGLRenderer(window);
        break;
#endif
#if VULKAN_SUPPORT
    case Video::Renderer::GraphicsAPI::VULKAN:
        renderer = new Video::VulkanRenderer(window);
        break;
#endif
#if WEBGPU_SUPPORT
    case Video::Renderer::GraphicsAPI::WEBGPU:
        renderer = new Video::WebGPURenderer(window);
        break;
#endif
    }

    // Test setup.
    Log(Log::INFO) << "Running " << test->GetName() << "\n";
    test->Setup(renderer, window->GetSize());
}

void RunFrame() {
#if !__ANDROID__
    glfwPollEvents();
#endif
    renderer->BeginFrame();
    theTest->Frame();
    renderer->Present();
}

void ShutdownTest() {
    renderer->Wait();
    theTest->Shutdown();
    delete renderer;
}
