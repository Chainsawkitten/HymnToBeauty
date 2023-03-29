#include "WebGPURenderer.hpp"

#include <Utility/Log.hpp>
#include <Utility/Window.hpp>
#if WEBGPU_BACKEND_DAWN
#include <dawn/dawn_proc.h>
#include <dawn/native/DawnNative.h>
#endif
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

#include "WebGPUBuffer.hpp"
#include "WebGPUVertexDescription.hpp"
#include "WebGPUGeometryBinding.hpp"
#include "WebGPUCommandBuffer.hpp"
#include "WebGPUShader.hpp"
#include "WebGPUShaderProgram.hpp"
#include "WebGPUTexture.hpp"
#include "WebGPUSampler.hpp"
#include "WebGPUGraphicsPipeline.hpp"
#include "WebGPUComputePipeline.hpp"
#include "WebGPURenderTargetAllocator.hpp"

#if ANDROID

#else
#include <GLFW/glfw3.h>
    #if defined(_WIN32) || defined(WIN32)
    #define GLFW_EXPOSE_NATIVE_WIN32
    #elif __APPLE__
    #define GLFW_EXPOSE_NATIVE_COCOA
    #elif __linux__
    /// @todo Wayland?
    #define GLFW_EXPOSE_NATIVE_X11
    #endif
#include <GLFW/glfw3native.h>
#endif

#include "PostProcessing.vert.hpp"
#include "SampleTexture.frag.hpp"

namespace Video {

WebGPURenderer::WebGPURenderer(::Utility::Window* window) {
    optionalFeatures = {};

    InitializeWebGPUBackend();
    CreateInstance();
    CreateSurface(window);
    RequestAdapter();
    CreateDevice();
    CreateSwapChain(window);
    CreateSamplers();

    renderTargetAllocator = new WebGPURenderTargetAllocator(*this);

    // Blitting.
    blitVertexShader = CreateShader(POSTPROCESSING_VERT, Shader::Type::VERTEX_SHADER);
    blitFragmentShader = CreateShader(SAMPLETEXTURE_FRAG, Shader::Type::FRAGMENT_SHADER);
    blitShaderProgram = CreateShaderProgram({blitVertexShader, blitFragmentShader});

    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;
    blitGraphicsPipeline = CreateGraphicsPipeline(blitShaderProgram, configuration);

    //TestRendering();
}

WebGPURenderer::~WebGPURenderer() {
    ReleaseTemporaryBuffers();

    for (uint32_t i = 0; i < static_cast<uint32_t>(Sampler::Filter::COUNT) * static_cast<uint32_t>(Sampler::Clamping::COUNT); ++i) {
        delete samplers[i];
    }
    delete renderTargetAllocator;

    delete blitGraphicsPipeline;
    delete blitShaderProgram;
    delete blitVertexShader;
    delete blitFragmentShader;

    wgpuSwapChainRelease(swapChain);
#if WEBGPU_BACKEND_DAWN
    wgpuQueueRelease(queue);
#endif
    wgpuDeviceRelease(device);
    wgpuAdapterRelease(adapter);
    wgpuSurfaceRelease(surface);
    wgpuInstanceRelease(instance);
}

CommandBuffer* WebGPURenderer::CreateCommandBuffer() {
    return new WebGPUCommandBuffer(this);
}

void WebGPURenderer::BeginFrame() {
    ReleaseTemporaryBuffers();

    // Acquire next swap chain image.
    currentFrame = wgpuSwapChainGetCurrentTextureView(swapChain);
    /// @todo Handle swap chain recreation.
}

void WebGPURenderer::Submit(CommandBuffer* commandBuffer) {
    assert(commandBuffer != nullptr);

    WebGPUCommandBuffer* webGPUCommandBuffer = static_cast<WebGPUCommandBuffer*>(commandBuffer);

    WGPUCommandBuffer cmdbuf = webGPUCommandBuffer->End();
    webGPUCommandBuffer->NextFrame();

    wgpuQueueSubmit(queue, 1, &cmdbuf);
#if WEBGPU_BACKEND_DAWN
    wgpuCommandBufferRelease(cmdbuf);
#endif
}

void WebGPURenderer::Present() {
    wgpuSwapChainPresent(swapChain);
}

Buffer* WebGPURenderer::CreateBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data) {
    return new WebGPUBuffer(device, bufferUsage, size, data);
}

Buffer* WebGPURenderer::CreateTemporaryBuffer(Buffer::BufferUsage bufferUsage, uint32_t size, const void* data) {
    WebGPUBuffer* buffer = new WebGPUBuffer(device, bufferUsage, size ,data);
    temporaryBuffers.push_back(buffer);
    return buffer;
}

VertexDescription* WebGPURenderer::CreateVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes, bool indexBuffer) {
    return new WebGPUVertexDescription(attributeCount, attributes);
}

GeometryBinding* WebGPURenderer::CreateGeometryBinding(const VertexDescription* vertexDescription, Buffer* vertexBuffer, GeometryBinding::IndexType indexType, const Buffer* indexBuffer) {
    return new WebGPUGeometryBinding(vertexBuffer, indexType, indexBuffer);
}

Shader* WebGPURenderer::CreateShader(const ShaderSource& shaderSource, Shader::Type type) {
    return new WebGPUShader(device, shaderSource, type);
}

ShaderProgram* WebGPURenderer::CreateShaderProgram(std::initializer_list<const Shader*> shaders) {
    return new WebGPUShaderProgram(device, shaders);
}

Texture* WebGPURenderer::CreateTexture(const glm::uvec2 size, Texture::Format format, int components, unsigned char* data) {
    assert(data != nullptr);

    return new WebGPUTexture(*this, size, Texture::Type::COLOR, format, components, data);
}

const Sampler* WebGPURenderer::GetSampler(Sampler::Filter filter, Sampler::Clamping clamping) const {
    assert(filter < Sampler::Filter::COUNT);
    assert(clamping < Sampler::Clamping::COUNT);

    return samplers[static_cast<uint32_t>(filter) * static_cast<uint32_t>(Sampler::Filter::COUNT) + static_cast<uint32_t>(clamping)];
}

Texture* WebGPURenderer::CreateRenderTarget(const glm::uvec2& size, Texture::Format format) {
    return renderTargetAllocator->CreateRenderTarget(size, format);
}

void WebGPURenderer::FreeRenderTarget(Texture* renderTarget) {
    renderTargetAllocator->FreeRenderTarget(renderTarget);
}

GraphicsPipeline* WebGPURenderer::CreateGraphicsPipeline(const ShaderProgram* shaderProgram, const GraphicsPipeline::Configuration& configuration, const VertexDescription* vertexDescription) {
    return new WebGPUGraphicsPipeline(*this, shaderProgram, configuration, vertexDescription);
}

ComputePipeline* WebGPURenderer::CreateComputePipeline(const ShaderProgram* shaderProgram) {
    return new WebGPUComputePipeline(device, shaderProgram);
}

void WebGPURenderer::Wait() {
#if WEBGPU_BACKEND_WGPU
    // wgpu-native doesn't currently implement wgpuQueueOnSubmittedWorkDone. Just wait for an arbitrary amount of time instead.
    /// @todo Remove once wgpu-native adds it.
    std::this_thread::sleep_for(std::chrono::duration<int>(1));
    return;
#endif

    std::atomic<bool> finished = false;
    wgpuQueueOnSubmittedWorkDone(
        queue,
#if WEBGPU_BACKEND_DAWN
        // Dawn additionally takes a signal value.
        /// @todo Remove after updating Dawn
        0,
#endif
        [](WGPUQueueWorkDoneStatus status, void* userdata) {
            *reinterpret_cast<bool*>(userdata) = true;
        }, &finished);

    while (!finished) {
        wgpuDeviceTick(device);
        std::this_thread::yield();
    }
}

unsigned char* WebGPURenderer::ReadImage(Texture* texture) {
    // Bytes per row needs to be 256 aligned.
    uint32_t bytesPerRow = texture->GetSize().x * 4;
    bytesPerRow = (bytesPerRow + 256 - 1) / 256 * 256;

    // Create buffer to copy data into, that can be mapped on the CPU.
    uint64_t bufferSize = bytesPerRow * texture->GetSize().y;
    WGPUBufferDescriptor bufferDescriptor = {};
    bufferDescriptor.size = bufferSize;
    bufferDescriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead;
    bufferDescriptor.mappedAtCreation = false;

    WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &bufferDescriptor);

    // Copy to buffer.
    CommandBuffer* cmdbuf = CreateCommandBuffer();

    WGPUCommandEncoder commandEncoder = static_cast<WebGPUCommandBuffer*>(cmdbuf)->GetCommandEncoder();

    WGPUImageCopyTexture source = {};
    source.texture = static_cast<WebGPUTexture*>(texture)->GetTexture();
    source.aspect = WGPUTextureAspect_All;

    WGPUImageCopyBuffer destination = {};
    destination.buffer = buffer;
    destination.layout.offset = 0;
    destination.layout.bytesPerRow = bytesPerRow;
    destination.layout.rowsPerImage = texture->GetSize().y;

    WGPUExtent3D copySize = {};
    copySize.width = texture->GetSize().x;
    copySize.height = texture->GetSize().y;
    copySize.depthOrArrayLayers = 1;

    wgpuCommandEncoderCopyTextureToBuffer(commandEncoder, &source, &destination, &copySize);

    Submit(cmdbuf);

    delete cmdbuf;

    // Read back buffer.
    std::atomic<bool> finished = false;
    wgpuBufferMapAsync(
        buffer, WGPUMapMode_Read, 0, bufferSize,
        [](WGPUBufferMapAsyncStatus status, void* userdata) {
            *reinterpret_cast<bool*>(userdata) = true;
        }, &finished);

    // Wait for mapping to finish.
    while (!finished) {
        // Submit empty command to "tick" the device.
        /// @todo Doesn't work for some reason. Need to call tick explicitly.
        //wgpuQueueSubmit(queue, 0, nullptr);

        wgpuDeviceTick(device);
        std::this_thread::yield();
    }

    // Copy buffer contents. Account for alignment.
    const unsigned char* memory = reinterpret_cast<const unsigned char*>(wgpuBufferGetConstMappedRange(buffer, 0, bufferSize));
    unsigned char* data = new unsigned char[texture->GetSize().x * texture->GetSize().y * 4];
    for (uint32_t y = 0; y < texture->GetSize().y; ++y) {
        memcpy(&data[texture->GetSize().x * y * 4], &memory[bytesPerRow * y], texture->GetSize().x * 4);
    }
    wgpuBufferUnmap(buffer);

    // Cleanup.
    wgpuBufferRelease(buffer);

    return data;
}

const std::vector<Profiling::Event>& WebGPURenderer::GetTimeline() const {
    return finishedEvents;
}

const WebGPURenderer::OptionalFeatures& WebGPURenderer::GetOptionalFeatures() const {
    return optionalFeatures;
}

WebGPUTexture* WebGPURenderer::CreateAttachmentlessRenderTarget(const glm::uvec2& size, uint32_t sampleCount) {
    return renderTargetAllocator->CreateAttachmentlessRenderTarget(size, sampleCount);
}

void WebGPURenderer::FreeAttachmentlessRenderTarget(WebGPUTexture* renderTarget) {
    renderTargetAllocator->FreeAttachmentlessRenderTarget(renderTarget);
}

WGPUDevice WebGPURenderer::GetDevice() const {
    return device;
}

WGPUQueue WebGPURenderer::GetQueue() const {
    return queue;
}

WGPUTextureView WebGPURenderer::GetCurrentFrame() const {
    return currentFrame;
}

GraphicsPipeline* WebGPURenderer::GetBlitGraphicsPipeline() const {
    return blitGraphicsPipeline;
}

bool WebGPURenderer::HasDepthClipControl() const {
    return depthClipControlEnabled;
}

bool WebGPURenderer::HasR11G11B10() const {
    return r11g11b10Enabled;
}

WGPUTextureFormat WebGPURenderer::GetSwapChainFormat() const {
    return swapChainFormat;
}

void WebGPURenderer::InitializeWebGPUBackend() {
#if WEBGPU_BACKEND_DAWN
    // Initialize Dawn
    DawnProcTable procs = dawn::native::GetProcs();
    dawnProcSetProcs(&procs);
#elif WEBGPU_BACKEND_WGPU
    /// @todo wgpu backend
#endif
}

void WebGPURenderer::CreateInstance() {
    WGPUInstanceDescriptor instanceDescriptor = {};
    instanceDescriptor.nextInChain = nullptr;

    instance = wgpuCreateInstance(&instanceDescriptor);
}

void WebGPURenderer::CreateSurface(::Utility::Window* window) {
#if ANDROID
    WGPUSurfaceDescriptorFromAndroidNativeWindow platformSurfaceDescriptor = {};
    platformSurfaceDescriptor.chain.next = nullptr;
    platformSurfaceDescriptor.chain.sType = WGPUSType_SurfaceDescriptorFromAndroidNativeWindow;
    platformSurfaceDescriptor.window = window->GetAndroidWindow();
#elif defined(_WIN32) || defined(WIN32)
    WGPUSurfaceDescriptorFromWindowsHWND platformSurfaceDescriptor = {};
    platformSurfaceDescriptor.chain.next = nullptr;
    platformSurfaceDescriptor.chain.sType = WGPUSType_SurfaceDescriptorFromWindowsHWND;
    platformSurfaceDescriptor.hinstance = GetModuleHandle(NULL);
    platformSurfaceDescriptor.hwnd = glfwGetWin32Window(window->GetGLFWWindow());
#elif __APPLE__
#error "WebGPU surface support has not been implemented on Mac."
    /// @todo Implement WGPUSurfaceDescriptorFromMetalLayer
#elif __linux__
    WGPUSurfaceDescriptorFromXlibWindow platformSurfaceDescriptor = {};
    platformSurfaceDescriptor.chain.next = nullptr;
    platformSurfaceDescriptor.chain.sType = WGPUSType_SurfaceDescriptorFromXlibWindow;
    platformSurfaceDescriptor.display = glfwGetX11Display();
    platformSurfaceDescriptor.window = glfwGetX11Window(window->GetGLFWWindow());
#else
#error "Unsupported platform"
#endif

    WGPUSurfaceDescriptor surfaceDescriptor = {};
    surfaceDescriptor.label = nullptr;
    surfaceDescriptor.nextInChain = reinterpret_cast<const WGPUChainedStruct*>(&platformSurfaceDescriptor);

    surface = wgpuInstanceCreateSurface(instance, &surfaceDescriptor);
}

void WebGPURenderer::RequestAdapter() {
    WGPURequestAdapterOptions options = {};
    options.compatibleSurface = surface;
    options.forceFallbackAdapter = false;
    options.powerPreference = WGPUPowerPreference_HighPerformance;

    struct UserData {
        WGPUAdapter adapter = nullptr;
        std::atomic<bool> finished = false;
    };
    UserData userData;

    wgpuInstanceRequestAdapter(
        instance, &options, [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const* message, void* userdata) {
            UserData* userData = reinterpret_cast<UserData*>(userdata);
            userData->adapter = adapter;
            userData->finished = true;
        }, &userData);

    // Wait for request to finish.
    while (!userData.finished) {
        std::this_thread::yield();
    };

    adapter = userData.adapter;

    // Get information about the adapter.
    WGPUAdapterProperties properties = {};
    wgpuAdapterGetProperties(adapter, &properties);
    
    Log(Log::INFO) << "Selected adapter " << properties.name << "\n";

    Log(Log::INFO) << "Backend type: ";
    switch (properties.backendType) {
    case WGPUBackendType_Null:
        Log(Log::INFO) << "Null.\n";
        break;
    case WGPUBackendType_WebGPU:
        Log(Log::INFO) << "WebGPU.\n";
        break;
    case WGPUBackendType_D3D11:
        Log(Log::INFO) << "D3D11.\n";
        break;
    case WGPUBackendType_D3D12:
        Log(Log::INFO) << "D3D12.\n";
        break;
    case WGPUBackendType_Metal:
        Log(Log::INFO) << "Metal.\n";
        break;
    case WGPUBackendType_Vulkan:
        Log(Log::INFO) << "Vulkan.\n";
        break;
    case WGPUBackendType_OpenGL:
        Log(Log::INFO) << "OpenGL.\n";
        break;
    case WGPUBackendType_OpenGLES:
        Log(Log::INFO) << "OpenGL ES.\n";
        break;
    }
}

void WebGPURenderer::CreateDevice() {
    std::vector<WGPUFeatureName> features;

    // Query features.
    /// @todo timestamp-query is currently experimental in Dawn.
    Log(Log::WARNING) << "timestamp-query not currently supported. GPU profiling will not be supported.\n";
    /* optionalFeatures.timestamps = wgpuAdapterHasFeature(adapter, WGPUFeatureName_TimestampQuery);
    if (optionalFeatures.timestamps) {
        features.push_back(WGPUFeatureName_TimestampQuery);
    } else {
        Log(Log::WARNING) << "timestamp-query not supported on the device. GPU profiling will not be supported.\n";
    }*/

    if (!wgpuAdapterHasFeature(adapter, WGPUFeatureName_RG11B10UfloatRenderable)) {
        Log(Log::WARNING) << "rg11b10ufloat-renderable is not supported. This will impact bandwidth usage.\n";
        r11g11b10Enabled = false;
    } else {
        features.push_back(WGPUFeatureName_RG11B10UfloatRenderable);
        r11g11b10Enabled = true;
    }

    if (!wgpuAdapterHasFeature(adapter, WGPUFeatureName_DepthClipControl)) {
        Log(Log::ERR) << "depth-clip-control is required but not supported on the adapter.\n";
        depthClipControlEnabled = false;
    } else {
        features.push_back(WGPUFeatureName_DepthClipControl);
        depthClipControlEnabled = true;
    }

    // WebGPU supports 1x and 4x MSAA.
    optionalFeatures.attachmentlessMsaaSamples = 1u | 4u;

#if WEBGPU_BACKEND_WGPU
    optionalFeatures.shaderAtomics = false;
#else
    optionalFeatures.shaderAtomics = true;
#endif

    WGPUDeviceDescriptor deviceDescriptor = {};
    deviceDescriptor.requiredFeaturesCount = features.size();
    deviceDescriptor.requiredFeatures = features.data();

    struct UserData {
        WGPUDevice device;
        std::atomic<bool> finished = false;
    };
    UserData userData;

    wgpuAdapterRequestDevice(
        adapter, &deviceDescriptor, [](WGPURequestDeviceStatus status, WGPUDevice device, char const* message, void* userdata) {
            UserData* userData = reinterpret_cast<UserData*>(userdata);
            userData->device = device;
            userData->finished = true;
        }, &userData);

    // Wait for request to finish.
    while (!userData.finished) {
        std::this_thread::yield();
    }

    device = userData.device;

    // Set error callbacks.
    wgpuDeviceSetDeviceLostCallback(
        device, [](WGPUDeviceLostReason reason, char const* message, void* userdata) {
            Log(Log::ERR) << "Device lost " << reason << ": " << message << "\n"; 
        }, nullptr);

    wgpuDeviceSetUncapturedErrorCallback(
        device, [](WGPUErrorType type, char const* message, void* userData) {
            Log(Log::ERR) << "Uncaptured device error " << type << ": " << message << "\n";
        },
        nullptr);

    // Get queue.
    queue = wgpuDeviceGetQueue(device);
}

void WebGPURenderer::CreateSwapChain(::Utility::Window* window) {
    swapChainSize = window->GetSize();

#if ANDROID
    swapChainFormat = WGPUTextureFormat_RGBA8Unorm;
#else
    swapChainFormat = WGPUTextureFormat_BGRA8Unorm;
#endif

    WGPUSwapChainDescriptor swapChainDescriptor = {};
    swapChainDescriptor.width = swapChainSize.x;
    swapChainDescriptor.height = swapChainSize.y;
    /// @todo Query swap chain format somehow?
    swapChainDescriptor.format = swapChainFormat;
    swapChainDescriptor.usage = WGPUTextureUsage_RenderAttachment;
    swapChainDescriptor.presentMode = WGPUPresentMode_Fifo;

    swapChain = wgpuDeviceCreateSwapChain(device, surface, &swapChainDescriptor);
}

void WebGPURenderer::CreateSamplers() {
    for (uint32_t i = 0; i < static_cast<uint32_t>(Sampler::Filter::COUNT) * static_cast<uint32_t>(Sampler::Clamping::COUNT); ++i) {
        samplers[i] = new WebGPUSampler(device,
                                        static_cast<Sampler::Filter>(i / static_cast<uint32_t>(Sampler::Filter::COUNT)),
                                        static_cast<Sampler::Clamping>(i % static_cast<uint32_t>(Sampler::Filter::COUNT)));
    }
}

void WebGPURenderer::ReleaseTemporaryBuffers() {
    for (WebGPUBuffer* buffer : temporaryBuffers) {
        delete buffer;
    }
    temporaryBuffers.clear();
}

} // namespace Video
