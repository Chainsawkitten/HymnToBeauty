#pragma once

#include <glm/glm.hpp>

namespace Video {

/// A render pass.
/**
 * @todo Better documentation. This is not an intuitive concept (and doesn't map neatly to OpenGL or Vulkan).
 */
class RenderPass {
  public:
    /// What to do with an image being rendered to before rendering to it.
    enum class LoadOperation {
        LOAD, ///< Load the previous image contents.
        CLEAR, ///< Clear the image contents.
        DONT_CARE ///< Don't care. The contents will be overwritten anyway.
    };

    /// Create new render pass.
    RenderPass() {}

    /// Destructor.
    virtual ~RenderPass() {}

    /// Get the size of the frame buffer.
    /**
     * @return The size of the frame buffer in pixels.
     */
    virtual const glm::uvec2& GetSize() const = 0;

  private:
    RenderPass(const RenderPass& other) = delete;
};

}
