#pragma once

#include "../Interface/LowLevelRenderer.hpp"

#include "WebGPU.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Utility {
class Window;
}

namespace Video {

class WebGPUBuffer;
class WebGPUSampler;
class WebGPURenderTargetAllocator;
class WebGPUTexture;

/// Low-level renderer implementing WebGPU.
class WebGPURenderer : public LowLevelRenderer {
  public:
    /// Create new WebGPU renderer.
    /**
     * @param window The window to render to.
     */
    explicit WebGPURenderer(::Utility::Window* window);

    /// Destructor.
    ~WebGPURenderer() final;

    CommandBuffer* CreateCommandBuffer() final;
    void BeginFrame() final;
    void Submit(CommandBuffer* commandBuffer) final;
    void Present() final;
    Buffer* CreateBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data = nullptr) final;
    Buffer* CreateTemporaryBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data = nullptr) final;
    VertexDescription* CreateVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes, bool indexBuffer = false) final;
    GeometryBinding* CreateGeometryBinding(const VertexDescription* vertexDescription, Buffer* vertexBuffer, GeometryBinding::IndexType indexType = GeometryBinding::IndexType::NONE, const Buffer* indexBuffer = nullptr) final;
    Shader* CreateShader(const ShaderSource& shaderSource, Shader::Type type) final;
    ShaderProgram* CreateShaderProgram(std::initializer_list<const Shader*> shaders) final;
    Texture* CreateTexture(const glm::uvec2 size, Texture::Format format, int components, unsigned char* data) final;
    const Sampler* GetSampler(Sampler::Filter filter, Sampler::Clamping clamping) const final;
    Texture* CreateRenderTarget(const glm::uvec2& size, Texture::Format format) final;
    void FreeRenderTarget(Texture* renderTarget) final;
    GraphicsPipeline* CreateGraphicsPipeline(const ShaderProgram* shaderProgram, const GraphicsPipeline::Configuration& configuration, const VertexDescription* vertexDescription = nullptr) final;
    ComputePipeline* CreateComputePipeline(const ShaderProgram* shaderProgram) final;
    void Wait() final;
    unsigned char* ReadImage(Texture* texture) final;
    const std::vector<Profiling::Event>& GetTimeline() const final;
    const OptionalFeatures& GetOptionalFeatures() const final;

    /// Create a render target to use for "attachmentless" rendering.
    /**
     * @param size The size of the render target.
     * @param sampleCount MSAA sample count.
     */
    WebGPUTexture* CreateAttachmentlessRenderTarget(const glm::uvec2& size, uint32_t sampleCount);

    /// Free an "attachmentless" render target.
    /**
     * @param renderTarget The render target to free.
     */
    void FreeAttachmentlessRenderTarget(WebGPUTexture* renderTarget);

    /// Get the WebGPU device.
    /**
     * @return The WebGPU device.
     */
    WGPUDevice GetDevice() const;

    /// Get the WebGPU queue.
    /**
     * @return The WebGPU queue.
     */
    WGPUQueue GetQueue() const;

    /// Get the texture view of the current frame's swap chain image.
    /**
     * @return The current frame's texture view.
     */
    WGPUTextureView GetCurrentFrame() const;

    /// Get the graphics pipeline used for blitting to the swap chain.
    /**
     * @return The graphics pipeline used for blitting.
     */
    GraphicsPipeline* GetBlitGraphicsPipeline() const;

    /// Get whether depth-clip-control can be used.
    /**
     * @return Whether the device has depth clip control.
     */
    bool HasDepthClipControl() const;

    /// Get the format of the swap chain.
    /**
     * @return The swap chain format.
     */
    WGPUTextureFormat GetSwapChainFormat() const;

  private:
    WebGPURenderer(const WebGPURenderer& other) = delete;

    void InitializeWebGPUBackend();
    void CreateInstance();
    void CreateSurface(::Utility::Window* window);
    void RequestAdapter();
    void CreateDevice();
    void CreateSwapChain(::Utility::Window* window);
    void CreateSamplers();

    void ReleaseTemporaryBuffers();

    WGPUInstance instance;
    WGPUSurface surface;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUSwapChain swapChain;
    WGPUQueue queue;
    WGPUTextureView currentFrame;

    WebGPUSampler* samplers[static_cast<uint32_t>(Sampler::Filter::COUNT) * static_cast<uint32_t>(Sampler::Clamping::COUNT)];
    
    WebGPURenderTargetAllocator* renderTargetAllocator;

    OptionalFeatures optionalFeatures;

    glm::uvec2 swapChainSize;
    WGPUTextureFormat swapChainFormat;

    std::vector<WebGPUBuffer*> temporaryBuffers;

    // Blitting to swap chain.
    Shader* blitVertexShader;
    Shader* blitFragmentShader;
    ShaderProgram* blitShaderProgram;
    GraphicsPipeline* blitGraphicsPipeline;

    std::vector<Profiling::Event> finishedEvents;

    bool depthClipControlEnabled;
};

} // namespace Video
