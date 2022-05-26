#include "Renderer.hpp"

#include "RenderScene.hpp"
#include "RenderProgram/StaticRenderProgram.hpp"
#include "RenderProgram/SpriteRenderProgram.hpp"
#include "Culling/AxisAlignedBoundingBox.hpp"
#include "Culling/Frustum.hpp"
#include "Culling/Sphere.hpp"
#include "Lighting/ZBinning.hpp"
#include "PostProcessing/PostProcessing.hpp"
#include "DebugDrawing.hpp"
#include "Texture/Texture2D.hpp"
#include "LowLevelRenderer/Interface/CommandBuffer.hpp"
#include "LowLevelRenderer/Interface/Shader.hpp"
#include "LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "LowLevelRenderer/Interface/Texture.hpp"
#include <Utility/Log.hpp>
#include <Utility/Profiling/Profiling.hpp>

#include "LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#ifdef OPENGL_SUPPORT
#include "LowLevelRenderer/OpenGL/OpenGLRenderer.hpp"
#endif
#ifdef VULKAN_SUPPORT
#include "LowLevelRenderer/Vulkan/VulkanRenderer.hpp"
#endif

#include "EditorEntity.vert.hpp"
#include "EditorEntity.frag.hpp"
#include "PostProcessing.vert.hpp"
#include "SampleTexture.frag.hpp"

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

    // Get the size of the window (and thus initial size of the output texture).
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    outputSize = glm::uvec2(width, height);

    commandBuffer = lowLevelRenderer->CreateCommandBuffer();

    staticRenderProgram = new StaticRenderProgram(lowLevelRenderer);
    spriteRenderProgram = new SpriteRenderProgram(lowLevelRenderer);

    zBinning = new ZBinning(lowLevelRenderer, outputSize);

    postProcessing = new PostProcessing(lowLevelRenderer);

    debugDrawing = new DebugDrawing(this);

	// Blitting.
	{
		blitVertexShader = lowLevelRenderer->CreateShader(POSTPROCESSING_VERT, Shader::Type::VERTEX_SHADER);
		blitFragmentShader = lowLevelRenderer->CreateShader(SAMPLETEXTURE_FRAG, Shader::Type::FRAGMENT_SHADER);
		blitShaderProgram = lowLevelRenderer->CreateShaderProgram({blitVertexShader, blitFragmentShader});

		GraphicsPipeline::Configuration configuration = {};
        configuration.primitiveType = PrimitiveType::TRIANGLE;
        configuration.polygonMode = PolygonMode::FILL;
        configuration.cullFace = CullFace::NONE;
        configuration.blendMode = BlendMode::NONE;
        configuration.depthMode = DepthMode::NONE;

		blitGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(blitShaderProgram, configuration);
	}

    sampler = lowLevelRenderer->GetSampler(Sampler::Filter::LINEAR, Sampler::Clamping::CLAMP_TO_EDGE);

    // Create quad geometry.
    {
        glm::vec2 vertex[6];
        vertex[0] = glm::vec2(0.0, 1.0);
        vertex[1] = glm::vec2(0.0, 0.0);
        vertex[2] = glm::vec2(1.0, 1.0);
        vertex[3] = glm::vec2(0.0, 0.0);
        vertex[4] = glm::vec2(1.0, 0.0);
        vertex[5] = glm::vec2(1.0, 1.0);

        quadVertexBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::VERTEX_BUFFER, sizeof(glm::vec2) * 6, &vertex);

        VertexDescription::Attribute attribute;
        attribute.size = 2;
        attribute.type = VertexDescription::AttributeType::FLOAT;
        attribute.normalized = false;

        quadVertexDescription = lowLevelRenderer->CreateVertexDescription(1, &attribute);
        quadGeometryBinding = lowLevelRenderer->CreateGeometryBinding(quadVertexDescription, quadVertexBuffer);
    }


	// Icon rendering.
	{
		iconVertexShader = lowLevelRenderer->CreateShader(EDITORENTITY_VERT, Shader::Type::VERTEX_SHADER);
		iconFragmentShader = lowLevelRenderer->CreateShader(EDITORENTITY_FRAG, Shader::Type::FRAGMENT_SHADER);
		iconShaderProgram = lowLevelRenderer->CreateShaderProgram({iconVertexShader, iconFragmentShader});

		GraphicsPipeline::Configuration configuration = {};
		configuration.primitiveType = PrimitiveType::TRIANGLE;
		configuration.polygonMode = PolygonMode::FILL;
		configuration.cullFace = CullFace::NONE;
		configuration.blendMode = BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA;
		configuration.depthMode = DepthMode::TEST;
		configuration.depthComparison = DepthComparison::LESS;

		iconGraphicsPipeline = lowLevelRenderer->CreateGraphicsPipeline(iconShaderProgram, configuration, quadVertexDescription);
	}
}

Renderer::~Renderer() {
    delete staticRenderProgram;
    delete spriteRenderProgram;

    delete zBinning;
    delete postProcessing;

	// Blitting.
    delete blitGraphicsPipeline;
    delete blitShaderProgram;
    delete blitVertexShader;
    delete blitFragmentShader;

    // Icon rendering.
    delete iconGraphicsPipeline;
    delete iconShaderProgram;
    delete iconVertexShader;
    delete iconFragmentShader;

    delete quadGeometryBinding;
    delete quadVertexDescription;
    delete quadVertexBuffer;

    delete commandBuffer;
    delete lowLevelRenderer;
}

LowLevelRenderer* Renderer::GetLowLevelRenderer() {
    return lowLevelRenderer;
}

void Renderer::SetOutputSize(const glm::uvec2& size) {
    outputSize = size;
}

const glm::uvec2& Renderer::GetRenderSurfaceSize() const {
    return renderSurfaceSize;
}

void Renderer::BeginFrame() {
    lowLevelRenderer->BeginFrame();
    outputTexture = lowLevelRenderer->CreateRenderTarget(outputSize, Texture::Format::R8G8B8A8);
}

void Renderer::Render(const RenderScene& renderScene) {
    bool firstCamera = true;

    for (const RenderScene::Camera& camera : renderScene.cameras) {
        PROFILE("Render camera");

		glm::uvec2 cameraOffset = glm::uvec2(glm::vec2(camera.viewport.x, camera.viewport.y) * glm::vec2(outputSize));
        glm::uvec2 cameraSize = glm::uvec2(glm::vec2(camera.viewport.z, camera.viewport.w) * glm::vec2(outputSize));
		SetRenderSurfaceSize(cameraSize);

        UpdateLights(renderScene, camera);

        std::vector<std::size_t> culledMeshes;
        culledMeshes = FrustumCulling(renderScene, camera);

		// Depth pre-pass
        depthTexture = lowLevelRenderer->CreateRenderTarget(renderSurfaceSize, Texture::Format::D32);
        RenderDepthPrePass(renderScene, culledMeshes, camera);

		// Main pass
        colorTexture = lowLevelRenderer->CreateRenderTarget(renderSurfaceSize, Texture::Format::R11G11B10);
        commandBuffer->BeginRenderPass(colorTexture, RenderPass::LoadOperation::CLEAR, depthTexture, RenderPass::LoadOperation::LOAD, "Main pass");

        staticRenderProgram->SetGamma(camera.postProcessingConfiguration.gamma);
        RenderOpaques(renderScene, culledMeshes, camera);
        RenderDebugShapes(renderScene, camera);
        spriteRenderProgram->SetGamma(camera.postProcessingConfiguration.gamma);
        RenderSprites(renderScene, camera);

        commandBuffer->EndRenderPass();

        lowLevelRenderer->FreeRenderTarget(depthTexture);

        // Post-processing and icons.
        {
            PROFILE("Post-processing");

            postProcessingTexture = lowLevelRenderer->CreateRenderTarget(renderSurfaceSize, Texture::Format::R8G8B8A8);
            postProcessing->Configure(camera.postProcessingConfiguration, postProcessingTexture);
            postProcessing->ApplyPostProcessing(*commandBuffer, colorTexture);
        }

        lowLevelRenderer->FreeRenderTarget(colorTexture);

        RenderIcons(renderScene, camera);

        commandBuffer->EndRenderPass();

        // Blit to output texture according to viewport.
        commandBuffer->BeginRenderPass(outputTexture, firstCamera ? RenderPass::LoadOperation::CLEAR : RenderPass::LoadOperation::LOAD, nullptr, RenderPass::LoadOperation::DONT_CARE, "Blit");
        commandBuffer->BindGraphicsPipeline(blitGraphicsPipeline);
        commandBuffer->SetViewportAndScissor(cameraOffset, cameraSize);
        commandBuffer->BindMaterial({{postProcessingTexture, sampler}});
        commandBuffer->Draw(3);
        commandBuffer->EndRenderPass();

        lowLevelRenderer->FreeRenderTarget(postProcessingTexture);

        firstCamera = false;
    }

    commandBuffer->BeginRenderPass(outputTexture, RenderPass::LoadOperation::LOAD, nullptr, RenderPass::LoadOperation::DONT_CARE, "GUI");
}

void Renderer::Present() {
    commandBuffer->EndRenderPass();
    commandBuffer->BlitToSwapChain(outputTexture);
    lowLevelRenderer->FreeRenderTarget(outputTexture);

    lowLevelRenderer->Submit(commandBuffer);

    lowLevelRenderer->Present();
}

void Renderer::WaitForRender() {
    lowLevelRenderer->Wait();
}

CommandBuffer* Renderer::GetCommandBuffer() {
    return commandBuffer;
}

void Renderer::SetRenderSurfaceSize(const glm::uvec2& size) {
    renderSurfaceSize = size;
    zBinning->SetRenderSurfaceSize(size);
}

void Renderer::UpdateLights(const RenderScene& renderScene, const RenderScene::Camera& camera) {
    PROFILE("Update lights");

    const Video::Frustum frustum(camera.viewProjectionMatrix);

    std::vector<DirectionalLight> directionalLightStructs;
    std::vector<Light> lights;

    // Add all directional lights.
    for (const RenderScene::DirectionalLight& directionalLight : renderScene.directionalLights) {
        glm::vec4 direction(directionalLight.direction, 0.0f);
        Video::DirectionalLight light;
        light.direction = camera.viewMatrix * -direction;
        light.intensitiesAmbientCoefficient = glm::vec4(directionalLight.color, directionalLight.ambientCoefficient);
        directionalLightStructs.push_back(light);
    }

    // Add all spot lights.
    /// @todo More accurate frustum culling.
    for (const RenderScene::SpotLight& spotLight : renderScene.spotLights) {
        Video::Sphere sphere(spotLight.position, spotLight.distance);

        if (frustum.Intersects(sphere)) {
            glm::vec4 direction(camera.viewMatrix * glm::vec4(spotLight.direction, 0.0f));
            glm::vec4 position(camera.viewMatrix * (glm::vec4(spotLight.position, 1.0f)));
            Video::Light light;
            light.positionDistance = glm::vec4(position.x, position.y, position.z, spotLight.distance);
            light.intensitiesAttenuation = glm::vec4(spotLight.color * spotLight.intensity, spotLight.attenuation);
            light.directionConeAngle = glm::vec4(direction.x, direction.y, direction.z, spotLight.coneAngle);
            lights.push_back(light);
        }
    }

    // Add all point lights.
    /// @todo More accurate frustum culling.
    for (const RenderScene::PointLight& pointLight : renderScene.pointLights) {
        Video::Sphere sphere(pointLight.position, pointLight.distance);

        if (frustum.Intersects(sphere)) {
            glm::vec4 position(camera.viewMatrix * (glm::vec4(pointLight.position, 1.0f)));
            Video::Light light;
            light.positionDistance = glm::vec4(position.x, position.y, position.z, pointLight.distance);
            light.intensitiesAttenuation = glm::vec4(pointLight.color * pointLight.intensity, pointLight.attenuation);
            light.directionConeAngle = glm::vec4(1.0f, 0.0f, 0.0f, 180.0f);
            lights.push_back(light);
        }
    }

    // Update light buffer.
    zBinning->BinLights(*commandBuffer, directionalLightStructs, lights, camera.projectionMatrix, camera.zNear, camera.zFar);
}

std::vector<std::size_t> Renderer::FrustumCulling(const RenderScene& renderScene, const RenderScene::Camera& camera) {
    PROFILE("Frustum culling");

    std::vector<std::size_t> culledMeshes;

    for (std::size_t i = 0; i < renderScene.meshes.size(); ++i) {
        Video::Frustum frustum(camera.viewProjectionMatrix * renderScene.meshes[i].modelMatrix);
        if (!frustum.Intersects(renderScene.meshes[i].axisAlignedBoundingBox))
            continue;

        culledMeshes.push_back(i);
    }

    return culledMeshes;
}

void Renderer::RenderDepthPrePass(const RenderScene& renderScene, const std::vector<std::size_t>& culledMeshes, const RenderScene::Camera& camera) {
    PROFILE("Depth pre-pass");

    commandBuffer->BeginRenderPass(nullptr, RenderPass::LoadOperation::DONT_CARE, depthTexture, RenderPass::LoadOperation::CLEAR, "Depth pre-pass");

    // Static meshes.
    if (!culledMeshes.empty()) {
        staticRenderProgram->PreDepthRender(*commandBuffer, camera.viewMatrix, camera.projectionMatrix);
        commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), renderSurfaceSize);

        for (std::size_t meshIndex : culledMeshes) {
            staticRenderProgram->DepthRender(*commandBuffer, renderScene.meshes[meshIndex].geometry, renderScene.meshes[meshIndex].modelMatrix);
        }
    }

    commandBuffer->EndRenderPass();
}

void Renderer::RenderOpaques(const RenderScene& renderScene, const std::vector<std::size_t>& culledMeshes, const RenderScene::Camera& camera) {
    PROFILE("Render opaques");

    // Static meshes.
    if (!culledMeshes.empty()) {
        staticRenderProgram->PreRender(*commandBuffer, camera.viewMatrix, camera.projectionMatrix, zBinning->GetLightInfo());
        commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), renderSurfaceSize);

        for (std::size_t meshIndex : culledMeshes) {
            staticRenderProgram->Render(*commandBuffer, renderScene.meshes[meshIndex].geometry, renderScene.meshes[meshIndex].albedo, renderScene.meshes[meshIndex].normal, renderScene.meshes[meshIndex].roughnessMetallic, renderScene.meshes[meshIndex].modelMatrix);
        }
    }
}

void Renderer::RenderDebugShapes(const RenderScene& renderScene, const RenderScene::Camera& camera) {
    PROFILE("Render debug entities");

    // Bind render target.
    debugDrawing->StartDebugDrawing(camera.viewProjectionMatrix);

    // Points.
    for (const DebugDrawing::Point& point : renderScene.debugDrawingPoints)
        debugDrawing->DrawPoint(point);

    // Lines.
    for (const DebugDrawing::Line& line : renderScene.debugDrawingLines)
        debugDrawing->DrawLine(line);

    // Cuboids.
    for (const DebugDrawing::Cuboid& cuboid : renderScene.debugDrawingCuboids)
        debugDrawing->DrawCuboid(cuboid);

    // Planes.
    for (const DebugDrawing::Plane& plane : renderScene.debugDrawingPlanes)
        debugDrawing->DrawPlane(plane);

    // Circles.
    for (const DebugDrawing::Circle& circle : renderScene.debugDrawingCircles)
        debugDrawing->DrawCircle(circle);

    // Spheres.
    for (const DebugDrawing::Sphere& sphere : renderScene.debugDrawingSpheres)
        debugDrawing->DrawSphere(sphere);

    // Cylinders.
    for (const DebugDrawing::Cylinder& cylinder : renderScene.debugDrawingCylinders)
        debugDrawing->DrawCylinder(cylinder);

    // Cones.
    for (const DebugDrawing::Cone& cone : renderScene.debugDrawingCones)
        debugDrawing->DrawCone(cone);

    // Meshes.
    for (const DebugDrawing::Mesh& mesh : renderScene.debugDrawingMeshes)
        debugDrawing->DrawMesh(mesh);

    debugDrawing->EndDebugDrawing();
}

void Renderer::RenderIcons(const RenderScene& renderScene, const RenderScene::Camera& camera) {
    if (!renderScene.icons.empty()) {
        PROFILE("Render icons");

        const glm::vec3 up(camera.viewMatrix[0][1], camera.viewMatrix[1][1], camera.viewMatrix[2][1]);
        PrepareRenderingIcons(camera.viewProjectionMatrix, camera.position, up);

        for (const RenderScene::Icon& icon : renderScene.icons) {
            commandBuffer->BindMaterial({{icon.texture->GetTexture(), sampler}});

            for (const glm::vec3& position : icon.positions) {
                RenderIcon(position);
            }
        }
    }
}

void Renderer::PrepareRenderingIcons(const glm::mat4& viewProjectionMatrix, const glm::vec3& cameraPosition, const glm::vec3& cameraUp) {
    commandBuffer->BindGraphicsPipeline(iconGraphicsPipeline);
    commandBuffer->BindGeometry(quadGeometryBinding);
    commandBuffer->SetViewportAndScissor(glm::uvec2(0, 0), renderSurfaceSize);

    // Set camera uniforms.
    Buffer* iconMatricesBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(glm::mat4), &viewProjectionMatrix);
    commandBuffer->BindUniformBuffer(ShaderProgram::BindingType::MATRICES, iconMatricesBuffer);

    this->cameraPosition = cameraPosition;
    this->cameraUp = cameraUp;
}

void Renderer::RenderIcon(const glm::vec3& position) {
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

void Renderer::RenderSprites(const RenderScene& renderScene, const RenderScene::Camera& camera) {
    PROFILE("Render sprites");

    if (!renderScene.sprites.empty()) {
        spriteRenderProgram->PreRender(*commandBuffer, camera.viewProjectionMatrix);

        for (const RenderScene::Sprite& sprite : renderScene.sprites) {
            spriteRenderProgram->Render(*commandBuffer, sprite.texture, sprite.size, sprite.pivot, sprite.modelMatrix, sprite.tint);
        }
    }
}
