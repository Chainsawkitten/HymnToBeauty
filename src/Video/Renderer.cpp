#include "Renderer.hpp"

#include "RenderProgram/StaticRenderProgram.hpp"
#include "PostProcessing/PostProcessing.hpp"
#include "Texture/Texture2D.hpp"
#include "LowLevelRenderer/Interface/CommandBuffer.hpp"
#include "LowLevelRenderer/Interface/Shader.hpp"
#include "LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "LowLevelRenderer/Interface/Texture.hpp"
#include "EditorEntity.vert.hpp"
#include "EditorEntity.frag.hpp"
#include <Utility/Log.hpp>

#include "LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#ifdef OPENGL_SUPPORT
#include "LowLevelRenderer/OpenGL/OpenGLRenderer.hpp"
#endif
#ifdef VULKAN_SUPPORT
#include "LowLevelRenderer/Vulkan/VulkanRenderer.hpp"
#endif

#include <GLFW/glfw3.h>

using namespace Video;

Renderer::Renderer(GraphicsAPI graphicsAPI, GLFWwindow* window) {
    // Create a low-level renderer of the selected graphics API.
    switch (graphicsAPI) {
#ifdef OPENGL_SUPPORT
    case GraphicsAPI::OPENGL:
        lowLevelRenderer = new OpenGLRenderer(window);
        break;
#endif
#ifdef VULKAN_SUPPORT
    case GraphicsAPI::VULKAN:
        lowLevelRenderer = new VulkanRenderer(window);
        break;
#endif
    }

    // Get the size of the window (and thus initial size of the render surface).
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    renderSurfaceSize = glm::uvec2(width, height);

    CreateRenderTextures();
    commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    staticRenderProgram = new StaticRenderProgram(lowLevelRenderer);

    postProcessing = new PostProcessing(lowLevelRenderer, postProcessingTexture);

    lightCount = 0;
    lightBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(Light) * MAX_LIGHTS);

    // Create icon geometry.
    glm::vec2 vertex[6];
    vertex[0] = glm::vec2(0.0, 1.0);
    vertex[1] = glm::vec2(0.0, 0.0);
    vertex[2] = glm::vec2(1.0, 1.0);
    vertex[3] = glm::vec2(0.0, 0.0);
    vertex[4] = glm::vec2(1.0, 0.0);
    vertex[5] = glm::vec2(1.0, 1.0);

    iconVertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER_STATIC, sizeof(glm::vec2) * 6, &vertex);

    VertexDescription::Attribute attribute;
    attribute.size = 2;
    attribute.type = VertexDescription::AttributeType::FLOAT;
    attribute.normalized = false;

    iconVertexDescription = lowLevelRenderer->CreateVertexDescription(1, &attribute);
    iconGeometryBinding = lowLevelRenderer->CreateGeometryBinding(iconVertexDescription, iconVertexBuffer);

    // Icon rendering.
    iconVertexShader = lowLevelRenderer->CreateShader(EDITORENTITY_VERT, Shader::Type::VERTEX_SHADER);
    iconFragmentShader = lowLevelRenderer->CreateShader(EDITORENTITY_FRAG, Shader::Type::FRAGMENT_SHADER);
    iconShaderProgram = lowLevelRenderer->CreateShaderProgram({iconVertexShader, iconFragmentShader});

    GraphicsPipeline::Configuration configuration;
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::NONE;
    configuration.blendMode = BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA;
    configuration.depthMode = DepthMode::TEST;
    configuration.depthComparison = DepthComparison::LESS;

    iconGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(iconShaderProgram, configuration, iconVertexDescription);

    iconMatricesBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(glm::mat4));
}

Renderer::~Renderer() {
    delete staticRenderProgram;

    delete postProcessing;

    delete lightBuffer;

    // Icon rendering.
    delete iconGraphicsPipeline;
    delete iconShaderProgram;
    delete iconVertexShader;
    delete iconFragmentShader;
    delete iconGeometryBinding;
    delete iconVertexDescription;
    delete iconVertexBuffer;
    delete iconMatricesBuffer;

    delete commandBuffer;
    FreeRenderTextures();
    delete postProcessingTexture;
    delete lowLevelRenderer;
}

LowLevelRenderer* Renderer::GetLowLevelRenderer() {
    return lowLevelRenderer;
}

void Renderer::SetRenderSurfaceSize(const glm::uvec2& size) {
    lowLevelRenderer->Wait();

    renderSurfaceSize = size;

    // Resize color and depth textures and recreate render passes.
    FreeRenderTextures();
    CreateRenderTextures();
    postProcessing->SetOutputTexture(postProcessingTexture);
}

const glm::uvec2& Renderer::GetRenderSurfaceSize() const {
    return renderSurfaceSize;
}

void Renderer::BeginFrame() {
    lowLevelRenderer->BeginFrame();
}

void Renderer::StartDepthPrePass() {
    commandBuffer->BeginRenderPass(depthRenderPass, "Depth pre-pass");
}

void Renderer::StartMainPass() {
    commandBuffer->EndRenderPass();
    commandBuffer->BeginRenderPass(mainRenderPass, "Main pass");
}

void Renderer::PrepareStaticMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    staticRenderProgram->PreDepthRender(*commandBuffer, viewMatrix, projectionMatrix);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), renderSurfaceSize);
}

void Renderer::DepthRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& modelMatrix) {
    staticRenderProgram->DepthRender(*commandBuffer, geometry, modelMatrix);
}

void Renderer::PrepareStaticMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float cameraNear, float cameraFar) {
    staticRenderProgram->PreRender(*commandBuffer, viewMatrix, projectionMatrix, lightBuffer, lightCount);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), renderSurfaceSize);
}

void Renderer::RenderStaticMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* roughnessMetallic, const glm::mat4 modelMatrix) {
    staticRenderProgram->Render(*commandBuffer, geometry, albedo, normal, roughnessMetallic, modelMatrix);
}

void Renderer::SetLights(const std::vector<Light>& lights) {
    lightCount = static_cast<unsigned int>(lights.size());

    // Skip if no lights.
    if (lightCount == 0)
        return;

    // Update light buffer.
    for (uint32_t i = 0; i < lightCount; ++i) {
        lightData[i] = lights[i];
    }

    lightBuffer->Write(lightData);
}

void Renderer::ApplyPostProcessing() {
    commandBuffer->EndRenderPass();
    postProcessing->ApplyPostProcessing(*commandBuffer, colorTexture);
}

void Renderer::Present() {
    commandBuffer->EndRenderPass();
    commandBuffer->BlitToSwapChain(postProcessingTexture);

    lowLevelRenderer->Submit(commandBuffer);

    lowLevelRenderer->Present();
}

void Renderer::WaitForRender() {
    lowLevelRenderer->Wait();
}

void Renderer::PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    currentIcon = nullptr;

    commandBuffer->BindGraphicsPipeline(iconGraphicsPipeline);
    commandBuffer->BindGeometry(iconGeometryBinding);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), renderSurfaceSize);

    // Set camera uniforms.
    iconMatricesBuffer->Write(&viewProjectionMatrix);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, iconMatricesBuffer);

    this->cameraPosition = cameraPosition;
    this->cameraUp = cameraUp;
}

void Renderer::RenderIcon(const glm::vec3& position, const Texture2D* icon) {
    if (currentIcon != icon) {
        currentIcon = icon;
        commandBuffer->BindMaterial({icon->GetTexture()});
    }

    struct PushConsts {
        glm::vec4 position;
        glm::vec4 right;
        glm::vec4 up;
    } pushConst;

    pushConst.position = glm::vec4(position, 0.0f);
    glm::vec3 look = cameraPosition - position;
    pushConst.right = glm::vec4(glm::normalize(glm::cross(cameraUp, look)), 0.0f);
    pushConst.up = glm::vec4(glm::normalize(glm::cross(look, glm::vec3(pushConst.right))), 0.0f);

    commandBuffer->PushConstants(&pushConst);
    commandBuffer->Draw(6);
}

void Renderer::ConfigurePostProcessing(const PostProcessing::Configuration& configuration) {
    postProcessing->Configure(configuration);
}

void Renderer::SetGamma(float gamma) {
    staticRenderProgram->SetGamma(gamma);
}

float Renderer::GetGamma() const {
    return staticRenderProgram->GetGamma();
}

CommandBuffer* Renderer::GetCommandBuffer() {
    return commandBuffer;
}

void Renderer::CreateRenderTextures() {
    colorTexture = lowLevelRenderer->CreateTexture(renderSurfaceSize, Texture::Type::RENDER_COLOR, Texture::Format::R11G11B10);
    depthTexture = lowLevelRenderer->CreateTexture(renderSurfaceSize, Texture::Type::RENDER_DEPTH, Texture::Format::D32);
    postProcessingTexture = lowLevelRenderer->CreateTexture(renderSurfaceSize, Texture::Type::RENDER_COLOR, Texture::Format::R8G8B8A8);
    depthRenderPass = lowLevelRenderer->CreateRenderPass(nullptr, RenderPass::LoadOperation::DONT_CARE, depthTexture, RenderPass::LoadOperation::CLEAR);
    mainRenderPass = lowLevelRenderer->CreateRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR, depthTexture, RenderPass::LoadOperation::LOAD);
    iconRenderPass = lowLevelRenderer->CreateRenderPass(postProcessingTexture, RenderPass::LoadOperation::LOAD);
}

void Renderer::FreeRenderTextures() {
    delete depthRenderPass;
    delete mainRenderPass;
    delete iconRenderPass;
    delete colorTexture;
    delete depthTexture;
}
