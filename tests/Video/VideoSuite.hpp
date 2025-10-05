#pragma once

#include <Framework/TestSuite.hpp>
#include <Video/Renderer.hpp>

namespace Video {
class LowLevelRenderer;
}
namespace Utility {
class Window;
}

/// Suite testing the Video module.
class VideoSuite : public TestSuite {
  public:
    /// Create a new video suite.
    /**
     * @param graphicsAPI Which graphics API to use.
     */
    explicit VideoSuite(Video::Renderer::GraphicsAPI graphicsAPI);
    
    /// Destructor.
    ~VideoSuite() {};

    /// Initialize the test suite.
    void Init();

    /// Shut down the test suite.
    void Shutdown();

    /// Width and height of the swapchain.
    static const unsigned int swapchainSize = 128;

  private:
    Video::Renderer::GraphicsAPI graphicsAPI;
    Video::LowLevelRenderer* lowLevelRenderer;

    Utility::Window* window;
};

/// Draw a colored triangle generated in the shader based on vertex index.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawTriangle(void* data);

/// Draw a colored triangle from a vertex buffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawVertexTriangle(void* data);

/// Draw a textured triangle.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawTexturedTriangle(void* data);

/// Draw a quad using a vertex and index buffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawQuad(void* data);

/// Draw two colored triangles, offset by a model matrix in uniform buffer and using a depth buffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawTriangles(void* data);

/// Draw two colored triangles, offset by a model matrix in push constants.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawPushTriangles(void* data);

/// Draw a triangle using vertex pulling from a storage buffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawStorageTriangle(void* data);

/// Draw two triangles, then invert the colors.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result InvertColors(void* data);

/// Draw a mipmapped triangle.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawMipmappedTriangle(void* data);

/// Use a depth pre-pass to occlude some fragments.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DepthPrePass(void* data);

/// Draw lines with width.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DrawLines(void* data);

/// Test attachmentless rendering.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result Attachmentless(void* data);

/// Test conservative rasterization.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result ConservativeRasterization(void* data);

/// Test depth clamping.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result DepthClamping(void* data);

/// Test rendering multiple frames.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result MultipleFrames(void* data);

/// Test setting buffer contents in a compute shader.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result ComputeSetBuffer(void* data);

/// Test writing vertex buffer contents in a compute shader.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result ComputeVertexBuffer(void* data);

/// Test using multiple storage buffers in a compute shaders.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result ComputeMultipleBuffers(void* data);

/// Test using ClearBuffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
Result ComputeClearBuffer(void* data);

Result DawnShader(void* data);
