#pragma once

#include "../Framework/SynchronizationTest.hpp"

#include <vulkan/vulkan.h>

namespace Video {
    class Texture;
    class CommandBuffer;
    class Shader;
    class ShaderProgram;
    class GraphicsPipeline;
    class VertexDescription;
}

class ValidationBug : public SynchronizationTest {
  public:
    const char* GetName() const override;
    void Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) override;
    void Frame() override;
    void Shutdown() override;

  private:
      Video::LowLevelRenderer* renderer;
      Video::CommandBuffer* commandBuffer;
      Video::Texture* mainRenderTarget;

      glm::uvec2 screenSize;

      VkDevice device;
      VkEvent event;
};
