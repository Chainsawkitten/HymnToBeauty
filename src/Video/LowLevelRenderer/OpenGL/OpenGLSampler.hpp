#pragma once

#include "../Interface/Sampler.hpp"

#include <glad/gl.h>

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
     * @param mipmap Whether mipmaps should be enabled.
     * 
     * @return The sampler.
     */
    GLuint GetSampler(bool mipmap) const;

  private:
    OpenGLSampler(const OpenGLSampler& other) = delete;

    GLuint sampler[2];
};

} // namespace Video
