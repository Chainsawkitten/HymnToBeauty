#include "RenderProgram.hpp"

using namespace Video;

RenderProgram::RenderProgram() {}

RenderProgram::~RenderProgram() {}

void RenderProgram::SetGamma(float gamma) {
    this->gamma = gamma;
}

float RenderProgram::GetGamma() const {
    return gamma;
}
