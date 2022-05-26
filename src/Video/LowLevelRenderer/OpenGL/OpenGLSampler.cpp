#include "OpenGLSampler.hpp"

#include <cassert>

namespace Video {

OpenGLSampler::OpenGLSampler(Filter filter, Clamping clamping) {
    assert(filter < Filter::COUNT);
    assert(clamping < Clamping::COUNT);

    glGenSamplers(1, &sampler);

    switch (filter) {
    case Filter::NEAREST:
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case Filter::LINEAR:
        glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    default:
        assert(false);
    }

    switch (clamping) {
    case Clamping::REPEAT:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
        break;
    case Clamping::CLAMP_TO_EDGE:
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        break;
    default:
        assert(false);
    }
}

OpenGLSampler::~OpenGLSampler() {}

GLuint OpenGLSampler::GetSampler() const {
    return sampler;
}

} // namespace Video
