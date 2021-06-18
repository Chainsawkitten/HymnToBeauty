#include "ZBinning.hpp"

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"
#include "../LowLevelRenderer/Interface/Buffer.hpp"
#include "../LowLevelRenderer/Interface/CommandBuffer.hpp"
#include "../LowLevelRenderer/Interface/Shader.hpp"
#include "../LowLevelRenderer/Interface/ShaderProgram.hpp"
#include "../LowLevelRenderer/Interface/ComputePipeline.hpp"
#include <assert.h>
#include <Utility/Log.hpp>

#include "LightBinning.comp.hpp"
#include "LightTiling.vert.hpp"
#include "LightTiling.frag.hpp"

namespace Video {

ZBinning::ZBinning(LowLevelRenderer* lowLevelRenderer, const glm::uvec2& screenSize) : isocahedron(lowLevelRenderer) {
    this->lowLevelRenderer = lowLevelRenderer;

    conservativeRasterization = lowLevelRenderer->GetOptionalFeatures().conservativeRasterization;

    // Create light buffers.
    lightInfo.directionalLightCount = 0;
    lightInfo.directionalLightBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(DirectionalLight) * MAX_DIRECTIONAL_LIGHTS);

    lightInfo.lightCount = 0;
    lightInfo.lightBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(Light) * MAX_LIGHTS);

    // Create binning resources.
    lightInfo.zBins = 128;
    lightInfo.zMaskBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(uint32_t) * MAX_LIGHTS / 32 * lightInfo.zBins);

    binningShader = lowLevelRenderer->CreateShader(LIGHTBINNING_COMP, Shader::Type::COMPUTE_SHADER);
    binningShaderProgram = lowLevelRenderer->CreateShaderProgram({ binningShader });
    binningPipeline = lowLevelRenderer->CreateComputePipeline(binningShaderProgram);

    // Create tiling resources.
    lightInfo.tileSize = 16;
    tilingRenderPass = nullptr;
    lightInfo.tileMaskBuffer = nullptr;
    if (!conservativeRasterization) {
        const uint32_t msaaSupport = lowLevelRenderer->GetOptionalFeatures().attachmentlessMsaaSamples;
        if (msaaSupport & 16) {
            msaa = 16;
            msaaScale = 4;
        } else if (msaaSupport & 4) {
            msaa = 4;
            msaaScale = 2;
        }
        assert(lightInfo.tileSize >= msaaScale);
    }
    SetRenderSurfaceSize(screenSize);

    lightInfo.maskCount = 0;

    tilingVertexShader = lowLevelRenderer->CreateShader(LIGHTTILING_VERT, Shader::Type::VERTEX_SHADER);
    tilingFragmentShader = lowLevelRenderer->CreateShader(LIGHTTILING_FRAG, Shader::Type::FRAGMENT_SHADER);
    tilingShaderProgram = lowLevelRenderer->CreateShaderProgram({ tilingVertexShader, tilingFragmentShader });

    tilingMatricesBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(TilingMatricesData));
    tilingUniformBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(TilingUniformData));

    GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = PrimitiveType::TRIANGLE;
    configuration.polygonMode = PolygonMode::FILL;
    configuration.cullFace = CullFace::FRONT;
    configuration.blendMode = BlendMode::NONE;
    configuration.depthMode = DepthMode::NONE;
    configuration.depthClamp = true;
    configuration.conservativeRasterization = conservativeRasterization;
    tilingPipeline = lowLevelRenderer->CreateGraphicsPipeline(tilingShaderProgram, configuration, isocahedron.GetVertexDescription());
}

ZBinning::~ZBinning() {
    delete lightInfo.directionalLightBuffer;
    delete lightInfo.lightBuffer;

    delete lightInfo.zMaskBuffer;

    delete binningPipeline;
    delete binningShaderProgram;
    delete binningShader;

    delete lightInfo.tileMaskBuffer;

    delete tilingRenderPass;

    delete tilingShaderProgram;
    delete tilingVertexShader;
    delete tilingFragmentShader;
    delete tilingPipeline;

    delete tilingMatricesBuffer;
    delete tilingUniformBuffer;
}

void ZBinning::SetRenderSurfaceSize(const glm::uvec2& size) {
    assert(size.x > 0 && size.y > 0);

    // Destroy old resources.
    if (tilingRenderPass != nullptr)
        delete tilingRenderPass;

    if (lightInfo.tileMaskBuffer != nullptr)
        delete lightInfo.tileMaskBuffer;

    screenSize = size;
    lightInfo.tiles = (screenSize - glm::uvec2(1, 1)) / glm::uvec2(lightInfo.tileSize, lightInfo.tileSize) + glm::uvec2(1, 1);

    // Create new resources.
    tilingSize = screenSize;
    if (conservativeRasterization) {
        tilingSize = lightInfo.tiles;
    } else {
        tilingSize /= msaaScale;
    }
    tilingRenderPass = lowLevelRenderer->CreateAttachmentlessRenderPass(tilingSize, msaa);

    lightInfo.tileMaskBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, lightInfo.tiles.x * lightInfo.tiles.y * sizeof(uint32_t) * MAX_LIGHTS / 32);
}

void ZBinning::BinLights(CommandBuffer& commandBuffer, const std::vector<DirectionalLight>& directionalLights, const std::vector<Light>& lights, const glm::mat4& projectionMatrix, float zNear, float zFar) {
    lightInfo.zNear = zNear;
    lightInfo.zFar = zFar;

    // Directional lights.
    lightInfo.directionalLightCount = static_cast<unsigned int>(directionalLights.size());
    if (lightInfo.directionalLightCount > MAX_DIRECTIONAL_LIGHTS) {
        Log(Log::WARNING) << "Max directional light count (" << MAX_DIRECTIONAL_LIGHTS << ") exceeded: " << lightInfo.directionalLightCount << ".\n";
        lightInfo.directionalLightCount = MAX_DIRECTIONAL_LIGHTS;
    }

    // Update directional light buffer.
    for (uint32_t i = 0; i < lightInfo.directionalLightCount; ++i) {
        directionalLightData[i] = directionalLights[i];
    }

    if (lightInfo.directionalLightCount > 0)
        lightInfo.directionalLightBuffer->Write(directionalLightData);

    // Spot and point lights.
    lightInfo.lightCount = static_cast<unsigned int>(lights.size());
    if (lightInfo.lightCount > MAX_LIGHTS) {
        Log(Log::WARNING) << "Max spot and point light count (" << MAX_LIGHTS << ") exceeded: " << lightInfo.lightCount << ".\n";
        lightInfo.lightCount = MAX_LIGHTS;
    }

    // Update light buffer.
    for (uint32_t i = 0; i < lightInfo.lightCount; ++i) {
        lightData[i] = lights[i];
    }

    if (lightInfo.lightCount > 0)
        lightInfo.lightBuffer->Write(lightData);

    lightInfo.maskCount = lightInfo.lightCount / 32 + (lightInfo.lightCount % 32 > 0);

    ClearBuffers(commandBuffer);

    if (lightInfo.lightCount > 0) {
        Binning(commandBuffer);
        Tiling(commandBuffer, projectionMatrix);
    }
}

const ZBinning::LightInfo& ZBinning::GetLightInfo() {
    return lightInfo;
}

void ZBinning::ClearBuffers(CommandBuffer& commandBuffer) {
    commandBuffer.ClearBuffer(lightInfo.zMaskBuffer);
    commandBuffer.ClearBuffer(lightInfo.tileMaskBuffer);
}

void ZBinning::Binning(CommandBuffer& commandBuffer) {
    commandBuffer.BindComputePipeline(binningPipeline);
    commandBuffer.BindStorageBuffers({ lightInfo.lightBuffer, lightInfo.zMaskBuffer });

    struct {
        uint32_t lightCount;
        uint32_t maskCount;
        uint32_t zBins;
        float zNear;
        float zFar;
    } pushConstants;

    pushConstants.lightCount = lightInfo.lightCount;
    pushConstants.maskCount = lightInfo.maskCount;
    pushConstants.zBins = lightInfo.zBins;
    pushConstants.zNear = lightInfo.zNear;
    pushConstants.zFar = lightInfo.zFar;
   
    commandBuffer.PushConstants(&pushConstants);
    const uint32_t threadGroupX = 64;
    commandBuffer.Dispatch(glm::uvec3((lightInfo.lightCount - 1) / threadGroupX + 1, 1, 1), "Binning");
}

void ZBinning::Tiling(CommandBuffer& commandBuffer, const glm::mat4& projectionMatrix) {
    // Determine the screen size represented by the rendered area.
    glm::uvec2 representativeTilingSize = tilingSize;
    if (conservativeRasterization) {
        representativeTilingSize *= lightInfo.tileSize;
    } else {
        representativeTilingSize *= msaaScale;
    }

    // Update uniform buffers.
    TilingMatricesData matricesData;
    matricesData.projectionMatrix = projectionMatrix;
    matricesData.scale = glm::vec2(screenSize) / glm::vec2(representativeTilingSize);
    matricesData.invIsocahedronError = 1.0f / isocahedron.GetError();

    tilingMatricesBuffer->Write(&matricesData);

    TilingUniformData uniformData;
    uniformData.tileSize = conservativeRasterization ? 1u : (lightInfo.tileSize / msaaScale);
    uniformData.maskCount = lightInfo.maskCount;
    uniformData.tilesX = lightInfo.tiles.x;

    tilingUniformBuffer->Write(&uniformData);

    // Draw proxy light geometry.
    commandBuffer.BeginRenderPass(tilingRenderPass, "Tiling");
    commandBuffer.BindGraphicsPipeline(tilingPipeline);
    commandBuffer.BindGeometry(isocahedron.GetGeometryBinding());
    commandBuffer.SetViewportAndScissor(glm::uvec2(0, 0), tilingSize);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::MATRICES, tilingMatricesBuffer);
    commandBuffer.BindUniformBuffer(ShaderProgram::BindingType::UNIFORMS, tilingUniformBuffer);
    commandBuffer.BindStorageBuffers({ lightInfo.lightBuffer, lightInfo.tileMaskBuffer });
    commandBuffer.DrawIndexedInstanced(isocahedron.GetIndexCount(), lightInfo.lightCount);
    commandBuffer.EndRenderPass();
}

}
