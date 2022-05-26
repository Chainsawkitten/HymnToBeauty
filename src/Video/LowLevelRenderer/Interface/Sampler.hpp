#pragma once

namespace Video {

/// Determines how a texture should be sampled.
class Sampler {
  public:
    /// The interpolation to apply.
    enum class Filter {
        NEAREST, ///< Nearest neighbor.
        LINEAR, ///< Bilinear filtering.
        COUNT ///< Number of entries in enum.
    };

    /// How to handle sampling outside the texture dimensions.
    enum class Clamping {
        REPEAT, ///< Repeat the texture.
        CLAMP_TO_EDGE, ///< Clamp to the edge of the texture.
        COUNT ///< Number of entries in enum.
    };

    /// Create a new sampler.
    Sampler() {}

    /// Destructor.
    virtual ~Sampler() {}

  private:
    Sampler(const Sampler& other) = delete;
};

} // namespace Video
