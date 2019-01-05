#pragma once

#include <glm/glm.hpp>

namespace Video {
class ShaderProgram;
class RenderSurface;
class Filter;
namespace Geometry {
class Rectangle;
}

/// Applies post-processing effects to the rendered image.
class PostProcessing {
  public:
    /// Create new post-processing handler.
    /**
     * @param rectangle %Rectangle to use for rendering.
     */
    explicit PostProcessing(const Geometry::Rectangle* rectangle);

    /// Destructor.
    ~PostProcessing();

    /// Apply a filter to the render image.
    /**
     * @param renderSurface %RenderSurface to apply filter to.
     * @param filter %Filter to apply.
     */
    void ApplyFilter(Video::RenderSurface* renderSurface, Filter* filter) const;

  private:
    PostProcessing(const PostProcessing& other) = delete;
    const Geometry::Rectangle* rectangle;
};
} // namespace Video
