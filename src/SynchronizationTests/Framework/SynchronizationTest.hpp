#pragma once

#include <glm/glm.hpp>

namespace Video {
    class LowLevelRenderer;
}

class SynchronizationTest {
  public:
    /// Get the name of the test.
    /**
     * @return The name of the test.
     */
    virtual const char* GetName() const = 0;

    /// Setup all internal resources.
    virtual void Setup(Video::LowLevelRenderer* renderer, const glm::uvec2& screenSize) = 0;

    /// Run one frame.
    virtual void Frame() = 0;

    /// Clean up all internal resources.
    virtual void Shutdown() = 0;

  private:
    
};
