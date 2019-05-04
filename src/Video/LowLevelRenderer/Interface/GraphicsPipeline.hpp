#pragma once

#include "Types.hpp"

namespace Video {

/// A graphics pipeline.
/**
 * @todo Write better documentation.
 */
class GraphicsPipeline {
  public:
    /// The configuration of the graphics pipeline.
    struct Configuration {
        /// The type of primitives to render.
        PrimitiveType primitiveType;

        /// How to render polygons.
        PolygonMode polygonMode;

        /// Which face to cull.
        CullFace cullFace;

        /// Blending mode.
        BlendMode blendMode;

        /// How to use depth buffer.
        DepthMode depthMode;

        /// How to perform depth comparisons.
        DepthComparison depthComparison;
    };

    /// Create a new graphics pipeline.
    GraphicsPipeline() {}

    /// Destructor.
    virtual ~GraphicsPipeline() {}

  private:
    GraphicsPipeline(const GraphicsPipeline& other) = delete;
};

}
