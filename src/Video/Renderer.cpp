#include "Renderer.hpp"

#include "RenderProgram/StaticRenderProgram.hpp"
#include "RenderProgram/SkinRenderProgram.hpp"
#include "RenderSurface.hpp"
#include "PostProcessing/PostProcessing.hpp"
#include "PostProcessing/FXAAFilter.hpp"
#include "Texture/Texture2D.hpp"
#include "Shader/Shader.hpp"
#include "Shader/ShaderProgram.hpp"
#include "EditorEntity.vert.hpp"
#include "EditorEntity.geom.hpp"
#include "EditorEntity.frag.hpp"
#include "Geometry/Rectangle.hpp"
#include "Buffer/FrameBuffer.hpp"
#include "Buffer/StorageBuffer.hpp"
#include "Buffer/ReadWriteTexture.hpp"

using namespace Video;

Renderer::Renderer() {
    rectangle = new Geometry::Rectangle();
    staticRenderProgram = new StaticRenderProgram();
    skinRenderProgram = new SkinRenderProgram();

    postProcessing = new PostProcessing(rectangle);

    fxaaFilter = new FXAAFilter();

    lightCount = 0;
    lightBuffer = new StorageBuffer(sizeof(Video::Light), GL_DYNAMIC_DRAW);

    // Icon rendering.
    Shader* iconVertexShader = new Shader(EDITORENTITY_VERT, EDITORENTITY_VERT_LENGTH, GL_VERTEX_SHADER);
    Shader* iconGeometryShader = new Shader(EDITORENTITY_GEOM, EDITORENTITY_GEOM_LENGTH, GL_GEOMETRY_SHADER);
    Shader* iconFragmentShader = new Shader(EDITORENTITY_FRAG, EDITORENTITY_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    iconShaderProgram = new ShaderProgram({iconVertexShader, iconGeometryShader, iconFragmentShader});
    delete iconVertexShader;
    delete iconGeometryShader;
    delete iconFragmentShader;

    // Get uniform locations.
    viewProjectionLocation = iconShaderProgram->GetUniformLocation("viewProjectionMatrix");
    cameraPositionLocation = iconShaderProgram->GetUniformLocation("cameraPosition");
    cameraUpLocation = iconShaderProgram->GetUniformLocation("cameraUp");
    baseImageLocation = iconShaderProgram->GetUniformLocation("baseImage");
    positionLocation = iconShaderProgram->GetUniformLocation("position");

    // Create icon geometry.
    float vertex;

    glBindVertexArray(0);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 1 * sizeof(float), &vertex, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(float), nullptr);

    glBindVertexArray(0);
}

Renderer::~Renderer() {
    delete rectangle;
    delete staticRenderProgram;
    delete skinRenderProgram;

    delete postProcessing;
    delete fxaaFilter;

    delete lightBuffer;

    // Icon rendering.
    delete iconShaderProgram;
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
}

void Renderer::StartRendering(RenderSurface* renderSurface) {
    renderSurface->Clear();
    glCullFace(GL_BACK);
    glViewport(0, 0, static_cast<GLsizei>(renderSurface->GetSize().x), static_cast<GLsizei>(renderSurface->GetSize().y));
}

void Renderer::PrepareStaticShadowRendering(const glm::mat4 lightView, glm::mat4 lightProjection, int shadowId, unsigned int shadowMapSize, int depthFbo) {
    staticRenderProgram->PreShadowRender(lightView, lightProjection, shadowId, shadowMapSize, depthFbo);
}

void Renderer::ShadowRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) {
    staticRenderProgram->ShadowRender(geometry, viewMatrix, projectionMatrix, modelMatrix);
}

void Renderer::PrepareStaticMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    staticRenderProgram->PreDepthRender(viewMatrix, projectionMatrix);
}

void Renderer::DepthRenderStaticMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix) {
    staticRenderProgram->DepthRender(geometry, viewMatrix, projectionMatrix, modelMatrix);
}

void Renderer::PrepareStaticMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float cameraNear, float cameraFar) {
    staticRenderProgram->PreRender(viewMatrix, projectionMatrix, lightBuffer, lightCount, cameraNear, cameraFar);
}

void Renderer::RenderStaticMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* metallic, const Texture2D* roughness, const glm::mat4 modelMatrix) {
    staticRenderProgram->Render(geometry, albedo, normal, metallic, roughness, modelMatrix);
}

void Renderer::PrepareSkinShadowRendering(const glm::mat4 lightView, glm::mat4 lightProjection, int shadowId, unsigned int shadowMapSize, int depthFbo) {
    skinRenderProgram->PreShadowRender(lightView, lightProjection, shadowId, shadowMapSize, depthFbo);
}

void Renderer::ShadowRenderSkinMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones) {
    skinRenderProgram->ShadowRender(geometry, viewMatrix, projectionMatrix, modelMatrix, bones);
}

void Renderer::PrepareSkinMeshDepthRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
    skinRenderProgram->PreDepthRender(viewMatrix, projectionMatrix);
}

void Renderer::DepthRenderSkinMesh(Geometry::Geometry3D* geometry, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix, const std::vector<glm::mat4>& bones) {
    skinRenderProgram->DepthRender(geometry, viewMatrix, projectionMatrix, modelMatrix, bones);
}

void Renderer::PrepareSkinMeshRendering(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, float cameraNear, float cameraFar) {
    skinRenderProgram->PreRender(viewMatrix, projectionMatrix, lightBuffer, lightCount, cameraNear, cameraFar);
}

void Renderer::RenderSkinMesh(Geometry::Geometry3D* geometry, const Texture2D* albedo, const Texture2D* normal, const Texture2D* metallic, const Texture2D* roughness, const glm::mat4 modelMatrix, const std::vector<glm::mat4>& bones) {
    skinRenderProgram->Render(geometry, albedo, normal, metallic, roughness, modelMatrix, bones);
}

void Renderer::SetLights(const std::vector<Video::Light>& lights) {
    lightCount = static_cast<unsigned int>(lights.size());

    // Skip if no lights.
    if (lightCount == 0)
        return;

    // Resize light buffer if necessary.
    unsigned int byteSize = sizeof(Video::Light) * static_cast<unsigned int>(lights.size());
    if (lightBuffer->GetSize() < byteSize) {
        delete lightBuffer;
        lightBuffer = new StorageBuffer(byteSize, GL_DYNAMIC_DRAW);
    }

    // Update light buffer.
    lightBuffer->Bind();
    lightBuffer->Write((void*)lights.data(), 0, byteSize);
    lightBuffer->Unbind();
}

void Renderer::AntiAlias(RenderSurface* renderSurface) {
    fxaaFilter->SetScreenSize(renderSurface->GetSize());
    postProcessing->ApplyFilter(renderSurface, fxaaFilter);
}

void Renderer::Present(RenderSurface* renderSurface, const glm::vec2& targetSize) {
    const glm::vec2 sourceSize = renderSurface->GetSize();

    // Copy color buffer.
    renderSurface->GetColorFrameBuffer()->BindRead();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    if (sourceSize == targetSize)
        glBlitFramebuffer(0, 0, sourceSize.x, sourceSize.y, 0, 0, targetSize.x, targetSize.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    else
        glBlitFramebuffer(0, 0, sourceSize.x, sourceSize.y, 0, 0, targetSize.x, targetSize.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    renderSurface->GetColorFrameBuffer()->Unbind();
}

void Renderer::PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    iconShaderProgram->Use();
    glBindVertexArray(vertexArray);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Set camera uniforms.
    glUniformMatrix4fv(viewProjectionLocation, 1, GL_FALSE, &viewProjectionMatrix[0][0]);
    glUniform3fv(cameraPositionLocation, 1, &cameraPosition[0]);
    glUniform3fv(cameraUpLocation, 1, &cameraUp[0]);
    glUniform1i(baseImageLocation, 0);
    glActiveTexture(GL_TEXTURE0);
}

void Renderer::RenderIcon(const glm::vec3& position, const Texture2D* icon) {
    if (currentIcon != icon) {
        currentIcon = icon;
        glBindTexture(GL_TEXTURE_2D, icon->GetTextureID());
    }

    glUniform3fv(positionLocation, 1, &position[0]);
    glDrawArrays(GL_POINTS, 0, 1);
}

void Renderer::StopRenderingIcons() {
    currentIcon = nullptr;
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void Renderer::SetGamma(float gamma) {
    staticRenderProgram->SetGamma(gamma);
    skinRenderProgram->SetGamma(gamma);
}

float Renderer::GetGamma() const {
    assert(staticRenderProgram->GetGamma() == skinRenderProgram->GetGamma());
    return staticRenderProgram->GetGamma();
}

void Renderer::SetFogApply(bool fogApply) {
    staticRenderProgram->SetFogApply(fogApply);
    skinRenderProgram->SetFogApply(fogApply);
}

bool Renderer::GetFogApply() const {
    assert(staticRenderProgram->GetFogApply() == skinRenderProgram->GetFogApply());
    return staticRenderProgram->GetFogApply();
}

void Renderer::SetFogDensity(float fogDensity) {
    staticRenderProgram->SetFogDensity(fogDensity);
    skinRenderProgram->SetFogDensity(fogDensity);
}

float Renderer::GetFogDensity() const {
    assert(staticRenderProgram->GetFogDensity() == skinRenderProgram->GetFogDensity());
    return staticRenderProgram->GetFogDensity();
}

void Renderer::SetFogColor(const glm::vec3& fogColor) {
    staticRenderProgram->SetFogColor(fogColor);
    skinRenderProgram->SetFogColor(fogColor);
}

glm::vec3 Renderer::GetFogColor() const {
    assert(staticRenderProgram->GetFogColor() == skinRenderProgram->GetFogColor());
    return staticRenderProgram->GetFogColor();
}

void Renderer::SetColorFilterApply(bool colorFilterApply) {
    staticRenderProgram->SetColorFilterApply(colorFilterApply);
    skinRenderProgram->SetColorFilterApply(colorFilterApply);
}

bool Renderer::GetColorFilterApply() const {
    assert(staticRenderProgram->GetColorFilterApply() == skinRenderProgram->GetColorFilterApply());
    return staticRenderProgram->GetColorFilterApply();
}

void Renderer::SetColorFilterColor(const glm::vec3& colorFilterColor) {
    staticRenderProgram->SetColorFilterColor(colorFilterColor);
    skinRenderProgram->SetColorFilterColor(colorFilterColor);
}

glm::vec3 Renderer::GetColorFilterColor() const {
    assert(staticRenderProgram->GetColorFilterColor() == skinRenderProgram->GetColorFilterColor());
    return staticRenderProgram->GetColorFilterColor();
}

void Renderer::SetDitherApply(bool ditherApply) {
    staticRenderProgram->SetDitherApply(ditherApply);
    skinRenderProgram->SetDitherApply(ditherApply);
}

bool Renderer::GetDitherApply() const {
    assert(staticRenderProgram->GetDitherApply() == skinRenderProgram->GetDitherApply());
    return staticRenderProgram->GetDitherApply();
}

void Renderer::SetFrameSize(const glm::vec2& frameSize) {
    staticRenderProgram->SetFrameSize(frameSize);
    skinRenderProgram->SetFrameSize(frameSize);
}

glm::vec2 Renderer::GetFrameSize() const {
    assert(staticRenderProgram->GetFrameSize() == skinRenderProgram->GetFrameSize());
    return staticRenderProgram->GetFrameSize();
}
