#pragma once

#include <glm/glm.hpp>

namespace Video {
/// A light used for lighting the scene.
struct Light {
    /// Position.
    glm::vec4 position;

    /// Intensity and attenuation.
    glm::vec4 intensitiesAttenuation;

    /// Direction and ambient coefficient.
    glm::vec4 directionAmbientCoefficient;

    /// Angle of the cone (in degrees).
    float coneAngle;

    /// Distance.
    float distance;

    /// Padding.
    float padding1;

    /// Padding.
    float padding2;
};
} // namespace Video
