#include "VideoSuite.hpp"

#include <iostream>
#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Utility/Window.hpp>

#ifdef OPENGL_SUPPORT
#include <Video/LowLevelRenderer/OpenGL/OpenGLRenderer.hpp>
#endif
#ifdef VULKAN_SUPPORT
#include <Video/LowLevelRenderer/Vulkan/VulkanRenderer.hpp>
#endif

#include <GLFW/glfw3.h>

VideoSuite::VideoSuite(Video::Renderer::GraphicsAPI graphicsAPI) : TestSuite("Video") {
    this->graphicsAPI = graphicsAPI;

    AddTest("DrawTriangle", DrawTriangle, &lowLevelRenderer);
    AddTest("DrawVertexTriangle", DrawVertexTriangle, &lowLevelRenderer);
    AddTest("DrawTexturedTriangle", DrawTexturedTriangle, &lowLevelRenderer);
    AddTest("DrawQuad", DrawQuad, &lowLevelRenderer);
    AddTest("DrawTriangles", DrawTriangles, &lowLevelRenderer);
    AddTest("DrawPushTriangles", DrawPushTriangles, &lowLevelRenderer);
    AddTest("DrawStorageTriangle", DrawStorageTriangle, &lowLevelRenderer);
    AddTest("InvertColors", InvertColors, &lowLevelRenderer);
    AddTest("DrawMipmappedTriangle", DrawMipmappedTriangle, &lowLevelRenderer);
    AddTest("DepthPrePass", DepthPrePass, &lowLevelRenderer);
    AddTest("DrawLines", DrawLines, &lowLevelRenderer);
    AddTest("Attachmentless", Attachmentless, &lowLevelRenderer);
    AddTest("ConservativeRasterization", ConservativeRasterization, &lowLevelRenderer);

    AddTest("MultipleFrames", MultipleFrames, &lowLevelRenderer);

    AddTest("ComputeSetBuffer", ComputeSetBuffer, &lowLevelRenderer);
    AddTest("ComputeVertexBuffer", ComputeVertexBuffer, &lowLevelRenderer);
    AddTest("ComputeMultipleBuffers", ComputeMultipleBuffers, &lowLevelRenderer);
}

void VideoSuite::Init() {
    glfwInit();

    switch (graphicsAPI) {
#ifdef OPENGL_SUPPORT
    case Video::Renderer::GraphicsAPI::OPENGL:
    {
        // Create window.
        window = new Utility::Window(swapchainSize, swapchainSize, false, false, "Tests - OpenGL", false);
        lowLevelRenderer = new Video::OpenGLRenderer(window);
        break;
    }
#endif
#ifdef VULKAN_SUPPORT
    case Video::Renderer::GraphicsAPI::VULKAN:
        // Create window.
        window = new Utility::Window(swapchainSize, swapchainSize, false, false, "Tests - Vulkan", true);
        lowLevelRenderer = new Video::VulkanRenderer(window);
        break;
#endif
    }
}

void VideoSuite::Shutdown() {
    lowLevelRenderer->Wait();
    delete lowLevelRenderer;
    delete window;
    glfwTerminate();
}
