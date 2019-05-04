#include "OpenGLRenderer.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Utility/Log.hpp>

#include "OpenGLCommandBuffer.hpp"
#include "OpenGLBuffer.hpp"
#include "OpenGLVertexDescription.hpp"
#include "OpenGLGeometryBinding.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLShaderProgram.hpp"
#include "OpenGLTexture.hpp"
#include "OpenGLRenderPass.hpp"
#include "OpenGLGraphicsPipeline.hpp"

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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

#ifndef NDEBUG
    glDebugMessageCallback(DebugMessageCallback, nullptr);
#endif

    // Blitting.
    blitVertexShader = CreateShader(POSTPROCESSING_VERT, Shader::Type::VERTEX_SHADER);
    blitFragmentShader = CreateShader(SAMPLETEXTURE_FRAG, Shader::Type::FRAGMENT_SHADER);
    blitShaderProgram = CreateShaderProgram({ blitVertexShader, blitFragmentShader });
}

OpenGLRenderer::~OpenGLRenderer() {
    delete blitShaderProgram;
    delete blitVertexShader;
    delete blitFragmentShader;
}

CommandBuffer* OpenGLRenderer::CreateCommandBuffer() {
    return new OpenGLCommandBuffer(*this);
}

void OpenGLRenderer::BeginFrame() {

}

void OpenGLRenderer::Submit(CommandBuffer* commandBuffer) {
    static_cast<OpenGLCommandBuffer*>(commandBuffer)->Submit();
}

void OpenGLRenderer::Present() {
    glfwSwapBuffers(window);
}

Buffer* OpenGLRenderer::CreateBuffer(Buffer::BufferUsage bufferUsage, unsigned int size, const void* data) {
    return new OpenGLBuffer(bufferUsage, size, data);
}

VertexDescription* OpenGLRenderer::CreateVertexDescription(unsigned int attributeCount, const VertexDescription::Attribute* attributes, bool indexBuffer) {
    return new OpenGLVertexDescription(attributeCount, attributes, indexBuffer);
}

GeometryBinding* OpenGLRenderer::CreateGeometryBinding(const VertexDescription* vertexDescription, const Buffer* vertexBuffer, GeometryBinding::IndexType indexType, const Buffer* indexBuffer) {
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

GraphicsPipeline* OpenGLRenderer::CreateGraphicsPipeline(const ShaderProgram* shaderProgram, const GraphicsPipeline::Configuration& configuration, const VertexDescription* vertexDescription) {
    return new OpenGLGraphicsPipeline(shaderProgram, configuration);
}

void OpenGLRenderer::Wait() {
    glFinish();
}

char* OpenGLRenderer::ReadImage(RenderPass* renderPass) {
    OpenGLRenderPass* openGLRenderPass = static_cast<OpenGLRenderPass*>(renderPass);

    // Calculate buffer size.
    const glm::uvec2 size = renderPass->GetSize();
    const uint32_t bufferSize = size.x * size.y * 4 * 1;

    // Read frame buffer contents.
    char* data = new char[bufferSize];
    glBindFramebuffer(GL_READ_FRAMEBUFFER, openGLRenderPass->GetFrameBuffer());
    glReadPixels(0, 0, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // Flip image.
    for (uint32_t y = 0; y < size.y / 2; ++y) {
        for (uint32_t x = 0; x < size.x; ++x) {
            for (uint32_t component = 0; component < 4; ++component) {
                char temp = data[(y * size.x + x) * 4 + component];
                data[(y * size.x + x) * 4 + component] = data[((size.y - y - 1) * size.x + x) * 4 + component];
                data[((size.y - y - 1) * size.x + x) * 4 + component] = temp;
            }
        }
    }

    return data;
}

const OpenGLShaderProgram* OpenGLRenderer::GetBlitShaderProgram() const {
    return static_cast<OpenGLShaderProgram*>(blitShaderProgram);
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
