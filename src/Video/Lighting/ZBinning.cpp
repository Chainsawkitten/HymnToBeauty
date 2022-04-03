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

    lightInfo.directionalLightCount = 0;
    lightInfo.lightCount = 0;

    // Create binning resources.
    lightInfo.zBins = 128;
    lightInfo.zMaskBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(uint32_t) * MAX_LIGHTS / 32 * lightInfo.zBins);

    binningShader = lowLevelRenderer->CreateShader(LIGHTBINNING_COMP, Shader::Type::COMPUTE_SHADER);
    binningShaderProgram = lowLevelRenderer->CreateShaderProgram({ binningShader });
    binningPipeline[0] = lowLevelRenderer->CreateComputePipeline(binningShaderProgram);
    binningPipeline[1] = lowLevelRenderer->CreateComputePipeline(binningShaderProgram);

    // Create tiling resources.
    lightInfo.tileSize = 16;
    lightInfo.tileMaskBuffer = lowLevelRenderer->CreateBuffer(Buffer::BufferUsage::STORAGE_BUFFER, TILE_MASK_BUFFER_SIZE);
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
    delete lightInfo.zMaskBuffer;
    delete lightInfo.tileMaskBuffer;

    delete binningPipeline[0];
    delete binningPipeline[1];
    delete binningShaderProgram;
    delete binningShader;

    delete tilingShaderProgram;
    delete tilingVertexShader;
    delete tilingFragmentShader;
    delete tilingPipeline;
}

void ZBinning::SetRenderSurfaceSize(const glm::uvec2& size) {
    assert(size.x > 0 && size.y > 0);

    screenSize = size;
    lightInfo.tiles = (screenSize - glm::uvec2(1, 1)) / glm::uvec2(lightInfo.tileSize, lightInfo.tileSize) + glm::uvec2(1, 1);

    tilingSize = screenSize;
    if (conservativeRasterization) {
        tilingSize = lightInfo.tiles;
    } else {
        tilingSize /= msaaScale;
    }
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

    lightInfo.directionalLightBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(DirectionalLight) * MAX_DIRECTIONAL_LIGHTS, directionalLightData);

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

    lightInfo.lightBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::STORAGE_BUFFER, sizeof(Light) * MAX_LIGHTS, lightData);

    lightInfo.maskCount = lightInfo.lightCount / 32 + (lightInfo.lightCount % 32 > 0);
    if (lightInfo.tiles.x * lightInfo.tiles.y * sizeof(uint32_t) * lightInfo.maskCount > TILE_MASK_BUFFER_SIZE) {
        Log(Log::ERR) << "Max tile mask buffer size exceeded.\n";
	}

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
    // Ping-pong pipelines to avoid an AMD bug.
    pipelineCount = 1u - pipelineCount;
    commandBuffer.BindComputePipeline(binningPipeline[pipelineCount]);
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

    Buffer* tilingMatricesBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(TilingMatricesData), &matricesData);

    TilingUniformData uniformData;
    uniformData.tileSize = conservativeRasterization ? 1u : (lightInfo.tileSize / msaaScale);
    uniformData.maskCount = lightInfo.maskCount;
    uniformData.tilesX = lightInfo.tiles.x;

    Buffer* tilingUniformBuffer = lowLevelRenderer->CreateTemporaryBuffer(Buffer::BufferUsage::UNIFORM_BUFFER, sizeof(TilingUniformData), &uniformData);

    // Draw proxy light geometry.
    commandBuffer.BeginAttachmentlessRenderPass(tilingSize, msaa, "Tiling");
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
