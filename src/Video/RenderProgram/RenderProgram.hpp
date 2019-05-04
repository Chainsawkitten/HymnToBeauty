#pragma once

#include <glm/glm.hpp>

namespace Video {
class Texture2D;
class ShaderProgram;
class StorageBuffer;
namespace Geometry {
class Geometry3D;
}

/// Super class render program.
class RenderProgram {
  public:
    /// Constructor.
    RenderProgram();

    /// Destructor.
    virtual ~RenderProgram();

    /// Gamma correction.
    /**
     * @param gamma Gamma to apply.
     */
    void SetGamma(float gamma);

    /// Gamma correction.
    /**
     * @return gamma Gamma value.
     */
    float GetGamma() const;

  protected:
    float gamma = 2.2f;
};
} // namespace Video
