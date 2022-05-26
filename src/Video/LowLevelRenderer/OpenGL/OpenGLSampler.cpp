#include "OpenGLSampler.hpp"

#include <cassert>

namespace Video {

OpenGLSampler::OpenGLSampler(Filter filter, Clamping clamping) {
    assert(filter < Filter::COUNT);
    assert(clamping < Clamping::COUNT);

    glGenSamplers(2, sampler);

    for (int i = 0; i < 2; ++i) {
        switch (filter) {
        case Filter::NEAREST:
            glSamplerParameteri(sampler[i], GL_TEXTURE_MIN_FILTER, i ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
            glSamplerParameteri(sampler[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case Filter::LINEAR:
            glSamplerParameteri(sampler[i], GL_TEXTURE_MIN_FILTER, i ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
            glSamplerParameteri(sampler[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        default:
            assert(false);
        }

        switch (clamping) {
        case Clamping::REPEAT:
            glSamplerParameteri(sampler[i], GL_TEXTURE_WRAP_S, GL_REPEAT);
            glSamplerParameteri(sampler[i], GL_TEXTURE_WRAP_T, GL_REPEAT);
            glSamplerParameteri(sampler[i], GL_TEXTURE_WRAP_R, GL_REPEAT);
            break;
        case Clamping::CLAMP_TO_EDGE:
            glSamplerParameteri(sampler[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glSamplerParameteri(sampler[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glSamplerParameteri(sampler[i], GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            break;
        default:
            assert(false);
        }
    }
}

OpenGLSampler::~OpenGLSampler() {}

GLuint OpenGLSampler::GetSampler(bool mipmap) const {
    return sampler[mipmap];
}

} // namespace Video
