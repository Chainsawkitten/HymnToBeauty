#include "OpenGLRenderer.hpp"

#include <GLFW/glfw3.h>
#include <Utility/Log.hpp>
#include <thread>

#include "OpenGLBuffer.hpp"
#include "OpenGLVertexDescription.hpp"
#include "OpenGLGeometryBinding.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLShaderProgram.hpp"
#include "OpenGLTexture.hpp"
#include "OpenGLRenderPass.hpp"
#include "OpenGLGraphicsPipeline.hpp"
#include "OpenGLComputePipeline.hpp"

#include "PostProcessing.vert.hpp"
#include "SampleTexture.frag.hpp"

namespace Video {

void HandleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam, bool showNotifications);
void APIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

OpenGLRenderer::OpenGLRenderer(GLFWwindow* window) {
    this->window = window;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        Log() << "Failed to initialize OpenGL context\n";

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#ifndef NDEBUG
    glDebugMessageCallback(DebugMessageCallback, nullptr);
#endif

    // Blitting.
    blitVertexShader = CreateShader(POSTPROCESSING_VERT, Shader::Type::VERTEX_SHADER);
    blitFragmentShader = CreateShader(SAMPLETEXTURE_FRAG, Shader::Type::FRAGMENT_SHADER);
    blitShaderProgram = CreateShaderProgram({ blitVertexShader, blitFragmentShader });

    // Generate queries.
    glGenQueries(maxQueries, queries);
    for (unsigned int i = 0; i < maxQueries; ++i) {
        freeQueries.push_back(queries[i]);
    }

    // Get optional features.
    GLint precision;
    glGetQueryiv(GL_TIMESTAMP, GL_QUERY_COUNTER_BITS, &precision);

    optionalFeatures = {};
    optionalFeatures.wideLines = true;
    optionalFeatures.timestamps = precision > 0;
    optionalFeatures.conservativeRasterization = GLAD_GL_NV_conservative_raster;

    assert(optionalFeatures.timestamps); /// @todo Disable GPU profiling if not supported.

    GLint maxSamples;
    glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &maxSamples);
    optionalFeatures.attachmentlessMsaaSamples = (static_cast<uint32_t>(maxSamples) << 1u) - 1u;
}

OpenGLRenderer::~OpenGLRenderer() {
    delete blitShaderProgram;
    delete blitVertexShader;
    delete blitFragmentShader;

    glDeleteQueries(maxQueries, queries);
}

CommandBuffer* OpenGLRenderer::CreateCommandBuffer() {
    return new OpenGLCommandBuffer(*this);
}

void OpenGLRenderer::BeginFrame() {
    firstSubmission = true;
}

void OpenGLRenderer::Submit(CommandBuffer* commandBuffer) {
    OpenGLCommandBuffer* openGLCommandBuffer = static_cast<OpenGLCommandBuffer*>(commandBuffer);

    // Get submitted queries.
    const std::vector<OpenGLCommandBuffer::Timing>& timings = openGLCommandBuffer->GetTimings();
    for (const OpenGLCommandBuffer::Timing& timing : timings) {
        submittedTimings[currentFrame].push_back(timing);
    }

    if (firstSubmission) {
        submissionTimes[currentFrame] = glfwGetTime();
        firstSubmission = false;
    }

    openGLCommandBuffer->Submit();
}

void OpenGLRenderer::Present() {
    glfwSwapBuffers(window);
    currentFrame = (currentFrame + 1) % buffering;

    // Handle queries.
    finishedEvents.clear();
    bool firstTimeSet = false;
    GLuint64 firstTime;

    for (const OpenGLCommandBuffer::Timing& timing : submittedTimings[currentFrame]) {
        // Queries should be available now.
        GLuint64 startAvailable = 0;
        GLuint64 endAvailable = 0;
        while (!startAvailable || !endAvailable) {
            glGetQueryObjectui64v(timing.startQuery, GL_QUERY_RESULT_AVAILABLE, &startAvailable);
            glGetQueryObjectui64v(timing.endQuery, GL_QUERY_RESULT_AVAILABLE, &endAvailable);
            std::this_thread::yield();
        }
            
        // Get query results.
        GLuint64 start, end;
        glGetQueryObjectui64v(timing.startQuery, GL_QUERY_RESULT, &start);
        glGetQueryObjectui64v(timing.endQuery, GL_QUERY_RESULT, &end);

        freeQueries.push_back(timing.startQuery);
        freeQueries.push_back(timing.endQuery);

        // Align with CPU timeline.
        if (!firstTimeSet) {
            firstTime = start;
            firstTimeSet = true;
        }

        // Write event info.
        Profiling::Event event(timing.name);
        event.time = submissionTimes[currentFrame] + static_cast<double>(start - firstTime) / 1000000000.0;
        event.duration = static_cast<double>(end - start) / 1000000000.0;

        finishedEvents.push_back(event);
    }
    submittedTimings[currentFrame].clear();
}

Buffer* OpenGLRenderer::CreateBuffer(Buffer::BufferUsage bufferUsage, unsigned int size, const void* data) {
    return new OpenGLBuffer(bufferUsage, size, data);
}

VertexDescription* OpenGLRenderer::CreateVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes, bool indexBuffer) {
    return new OpenGLVertexDescription(attributeCount, attributes, indexBuffer);
}

GeometryBinding* OpenGLRenderer::CreateGeometryBinding(const VertexDescription* vertexDescription, Buffer* vertexBuffer, GeometryBinding::IndexType indexType, const Buffer* indexBuffer) {
    return new OpenGLGeometryBinding(vertexDescription, vertexBuffer, indexType, indexBuffer);
}

Shader* OpenGLRenderer::CreateShader(const ShaderSource& shaderSource, Shader::Type type) {
    return new OpenGLShader(shaderSource, type);
}

ShaderProgram* OpenGLRenderer::CreateShaderProgram(std::initializer_list<const Shader*> shaders) {
    return new OpenGLShaderProgram(shaders);
}

Texture* OpenGLRenderer::CreateTexture(const glm::uvec2 size, Texture::Type type, Texture::Format format, int components, unsigned char* data) {
    return new OpenGLTexture(size, type, format, components, data);
}

RenderPass* OpenGLRenderer::CreateRenderPass(Texture* colorAttachment, RenderPass::LoadOperation colorLoadOperation, Texture* depthAttachment, RenderPass::LoadOperation depthLoadOperation) {
    return new OpenGLRenderPass(colorAttachment, colorLoadOperation, depthAttachment, depthLoadOperation);
}

RenderPass* OpenGLRenderer::CreateAttachmentlessRenderPass(const glm::uvec2& size, uint32_t msaaSamples) {
    return new OpenGLRenderPass(size, msaaSamples);
}

GraphicsPipeline* OpenGLRenderer::CreateGraphicsPipeline(const ShaderProgram* shaderProgram, const GraphicsPipeline::Configuration& configuration, const VertexDescription* vertexDescription) {
    return new OpenGLGraphicsPipeline(shaderProgram, configuration);
}

ComputePipeline* OpenGLRenderer::CreateComputePipeline(const ShaderProgram* shaderProgram) {
    return new OpenGLComputePipeline(shaderProgram);
}

void OpenGLRenderer::Wait() {
    glFinish();
}

unsigned char* OpenGLRenderer::ReadImage(RenderPass* renderPass) {
    OpenGLRenderPass* openGLRenderPass = static_cast<OpenGLRenderPass*>(renderPass);

    // Calculate buffer size.
    const glm::uvec2 size = renderPass->GetSize();
    const uint32_t bufferSize = size.x * size.y * 4 * 1;

    // Read frame buffer contents.
    unsigned char* data = new unsigned char[bufferSize];
    glBindFramebuffer(GL_READ_FRAMEBUFFER, openGLRenderPass->GetFrameBuffer());
    glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // Flip image.
    for (uint32_t y = 0; y < size.y / 2; ++y) {
        for (uint32_t x = 0; x < size.x; ++x) {
            for (uint32_t component = 0; component < 4; ++component) {
                unsigned char temp = data[(y * size.x + x) * 4 + component];
                data[(y * size.x + x) * 4 + component] = data[((size.y - y - 1) * size.x + x) * 4 + component];
                data[((size.y - y - 1) * size.x + x) * 4 + component] = temp;
            }
        }
    }

    return data;
}

const std::vector<Profiling::Event>& OpenGLRenderer::GetTimeline() const {
    return finishedEvents;
}

const OpenGLRenderer::OptionalFeatures& OpenGLRenderer::GetOptionalFeatures() const {
    return optionalFeatures;
}

const OpenGLShaderProgram* OpenGLRenderer::GetBlitShaderProgram() const {
    return static_cast<OpenGLShaderProgram*>(blitShaderProgram);
}

GLuint OpenGLRenderer::GetFreeQuery() {
    assert(!freeQueries.empty());

    GLuint query = freeQueries.back();
    freeQueries.pop_back();
    return query;
}

void HandleDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam, bool showNotifications) {
    if (!showNotifications && severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    switch (source) {
    case GL_DEBUG_SOURCE_API:
        fputs("Open GL API", stderr);
        break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        fputs("Window System", stderr);
        break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
        fputs("Shader Compiler", stderr);
        break;
    case GL_DEBUG_SOURCE_APPLICATION:
        fputs("Application", stderr);
        break;
    default:
        fputs("Other", stderr);
    }

    fputs(": ", stderr);

    switch (type) {
    case GL_DEBUG_TYPE_ERROR:
        fputs("Error", stderr);
        break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        fputs("Deprecated Behavior", stderr);
        break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        fputs("Undefined Behavior", stderr);
        break;
    case GL_DEBUG_TYPE_PORTABILITY:
        fputs("Portability", stderr);
        break;
    case GL_DEBUG_TYPE_PERFORMANCE:
        fputs("Performance", stderr);
        break;
    case GL_DEBUG_TYPE_MARKER:
        fputs("Marker", stderr);
        break;
    case GL_DEBUG_TYPE_PUSH_GROUP:
        fputs("Push Group", stderr);
        break;
    case GL_DEBUG_TYPE_POP_GROUP:
        fputs("Pop Group", stderr);
        break;
    default:
        fputs("Other", stderr);
    }

    fputs(" (", stderr);

    switch (severity) {
    case GL_DEBUG_SEVERITY_HIGH:
        fputs("High Priority", stderr);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        fputs("Medium Priority", stderr);
        break;
    case GL_DEBUG_SEVERITY_LOW:
        fputs("Low Priority", stderr);
        break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
        fputs("Notification", stderr);
        break;
    }

    fputs("):\n", stderr);

    fputs(message, stderr);
    fputs("\n\n", stderr);

    fflush(stderr);
}

void APIENTRY DebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    HandleDebugMessage(source, type, id, severity, length, message, userParam, false);
}

}
