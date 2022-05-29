#pragma once

namespace Video {

/// Which face to cull.
enum class CullFace {
    NONE, ///< Don't perform face culling.
    BACK, ///< Cull back faces.
    FRONT ///< Cull front faces.
};

/// How to use depth buffer.
enum class DepthMode {
    NONE, ///< Neither test against nor write to depth buffer.
    TEST, ///< Test depth, but don't write to depth buffer.
    TEST_WRITE, ///< Test depth and write to depth buffer.
};

/// How to perform depth comparisons.
enum class DepthComparison {
    LESS, ///< Passes if incoming value is less than stored value.
    LESS_EQUAL, ///< Passes if incoming value is less than or equal to stored value.
    ALWAYS, ///< Always passes.
};

/// How polygons are rasterized.
enum class PolygonMode {
    FILL, ///< Fill polygons.
    LINE, ///< Draw boundary edges as lines.
};

/// Filtering.
enum class Filtering {
    NEAREST_NEIGHBOR, ///< Nearest neighbor filtering.
    LINEAR, ///< Linear filtering.
};

/// Blending mode.
enum class BlendMode {
    NONE, ///< Don't use blending.
    ALPHA_ONE_MINUS_SRC_ALPHA, ///< Regular blending.
    ONE_ONE_MINUS_SRC_ALPHA ///< Pre-multiplied alpha.
};

/// The type of primitives to render.
enum class PrimitiveType {
    POINT, ///< Point.
    LINE, ///< Line.
    TRIANGLE ///< Triangle.
};

}
