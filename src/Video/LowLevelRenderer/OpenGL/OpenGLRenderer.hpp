#pragma once

#include "../Interface/LowLevelRenderer.hpp"

#include "OpenGLCommandBuffer.hpp"
#include <glad/glad.h>

struct GLFWwindow;

namespace Video {

class OpenGLShaderProgram;
class OpenGLBufferAllocator;
class OpenGLRenderPassAllocator;
class OpenGLRenderTargetAllocator;
class OpenGLSampler;

/// Low-level renderer implementing OpenGL.
class OpenGLRenderer : public LowLevelRenderer {
  public:
    /// Create new OpenGL renderer.
    /**
     * @param window The window to render to.
     */
    explicit OpenGLRenderer(GLFWwindow* window);

    /// Destructor.
    ~OpenGLRenderer() final;

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

    /// Get the shader program used for blitting.
    /**
     * @return The blit shader program.
     */
    const OpenGLShaderProgram* GetBlitShaderProgram() const;

    /// Get a free query.
    /**
     * @return The query object.
     */
    GLuint GetFreeQuery();

  private:
    OpenGLRenderer(const OpenGLRenderer& other) = delete;

    GLFWwindow* window;

    // Blitting to swap chain.
    Shader* blitVertexShader;
    Shader* blitFragmentShader;
    ShaderProgram* blitShaderProgram;

    OpenGLBufferAllocator* bufferAllocator;
    OpenGLRenderPassAllocator* renderPassAllocator;
    OpenGLRenderTargetAllocator* renderTargetAllocator;

    OpenGLSampler* samplers[static_cast<uint32_t>(Sampler::Filter::COUNT) * static_cast<uint32_t>(Sampler::Clamping::COUNT)];

    OptionalFeatures optionalFeatures;

    static const unsigned int buffering = 3;
    std::vector<Profiling::Event> finishedEvents;
    unsigned int currentFrame = 0;
    static const unsigned int maxQueries = buffering * 2 * 50;
    GLuint queries[maxQueries];
    std::vector<GLuint> freeQueries;
    std::vector<OpenGLCommandBuffer::Timing> submittedTimings[buffering];

    bool firstSubmission;
    double submissionTimes[buffering];
};

} // namespace Video
