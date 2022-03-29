#pragma once

#include <Framework/TestSuite.hpp>
#include <Video/Renderer.hpp>

struct GLFWwindow;
namespace Video {
class LowLevelRenderer;
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

    GLFWwindow* window;
};

/// Draw a colored triangle generated in the shader based on vertex index.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawTriangle(void* data);

/// Draw a colored triangle from a vertex buffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawVertexTriangle(void* data);

/// Draw a textured triangle.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawTexturedTriangle(void* data);

/// Draw a quad using a vertex and index buffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawQuad(void* data);

/// Draw two colored triangles, offset by a model matrix in uniform buffer and using a depth buffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawTriangles(void* data);

/// Draw two colored triangles, offset by a model matrix in push constants.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawPushTriangles(void* data);

/// Draw a triangle using vertex pulling from a storage buffer.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawStorageTriangle(void* data);

/// Draw two triangles, then invert the colors.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool InvertColors(void* data);

/// Draw a mipmapped triangle.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawMipmappedTriangle(void* data);

/// Use a depth pre-pass to occlude some fragments.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DepthPrePass(void* data);

/// Draw lines with width.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool DrawLines(void* data);

/// Test attachmentless rendering.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool Attachmentless(void* data);

/// Test conservative rasterization.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool ConservativeRasterization(void* data);

/// Test rendering multiple frames.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool MultipleFrames(void* data);

/// Test setting buffer contents in a compute shader.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool ComputeSetBuffer(void* data);

/// Test writing vertex buffer contents in a compute shader.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool ComputeVertexBuffer(void* data);

/// Test using multiple storage buffers in a compute shaders.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool ComputeMultipleBuffers(void* data);

/// Reproduces for vkCmdFillBuffer bug.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool BugFillBuffer(void* data);

/// Reproduces for vkCmdFillBuffer bug.
/**
 * @param data The low-level renderer.
 *
 * @return Whether the test was successful.
 */
bool BugFillBufferLoop(void* data);
