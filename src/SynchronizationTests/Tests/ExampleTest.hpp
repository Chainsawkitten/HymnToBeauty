#pragma once

#include "../Framework/SynchronizationTest.hpp"

namespace Video {
    class Texture;
    class CommandBuffer;
}

class ExampleTest : public SynchronizationTest {
  public:
    const char* GetName() const override;
    void Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) override;
    void Frame() override;
    void Shutdown() override;

  private:
      Video::LowLevelRenderer* renderer;
      Video::Texture* mainRenderTarget;
      Video::CommandBuffer* commandBuffer;
};
