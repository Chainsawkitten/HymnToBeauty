#include "RenderProgram.hpp"

using namespace Video;

RenderProgram::RenderProgram() {

}

RenderProgram::~RenderProgram() {

}

void RenderProgram::SetGamma(float gamma) {
    this->gamma = gamma;
}

float RenderProgram::GetGamma() const {
    return gamma;
}

void RenderProgram::SetFogApply(bool fogApply) {
    this->fogApply = (int)fogApply;
}

bool RenderProgram::GetFogApply() const {
    return (bool)fogApply;
}

void RenderProgram::SetFogDensity(float fogDensity) {
    this->fogDensity = fogDensity;
}

float RenderProgram::GetFogDensity() const {
    return fogDensity;
}

void RenderProgram::SetFogColor(const glm::vec3& fogColor) {
    this->fogColor = fogColor;
}

glm::vec3 RenderProgram::GetFogColor() const {
    return fogColor;
}

void RenderProgram::SetColorFilterApply(bool colorFilterApply) {
    this->colorFilterApply = (int)colorFilterApply;
}

bool RenderProgram::GetColorFilterApply() const {
    return (bool)colorFilterApply;
}

void RenderProgram::SetColorFilterColor(const glm::vec3& colorFilterColor) {
    this->colorFilterColor = colorFilterColor;
}

glm::vec3 RenderProgram::GetColorFilterColor() const {
    return colorFilterColor;
}

void RenderProgram::SetDitherApply(bool ditherApply) {
    this->ditherApply = (int)ditherApply;
}

bool RenderProgram::GetDitherApply() const {
    return (bool)ditherApply;
}

void RenderProgram::SetFrameSize(const glm::vec2& frameSize) {
    this->frameSize = frameSize;
}

glm::vec2 RenderProgram::GetFrameSize() const {
    return frameSize;
}
