#pragma once

#include "../Interface/Sampler.hpp"

#include <glad/glad.h>

namespace Video {

/// OpenGL implementation of Sampler.
class OpenGLSampler : public Sampler {
  public:
    /// Create new OpenGL sampler.
    /**
     * @param filter The interpolation to apply.
     * @param clamping How to handle sampling outside the texture dimensions.
     */
    OpenGLSampler(Filter filter, Clamping clamping);

    /// Destructor.
    ~OpenGLSampler() final;

    /// Get the sampler.
    /**
     * @return The sampler.
     */
    GLuint GetSampler() const;

  private:
    OpenGLSampler(const OpenGLSampler& other) = delete;

    GLuint sampler = 0;
};

} // namespace Video
