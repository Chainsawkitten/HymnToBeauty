#pragma once

#include <glm/glm.hpp>

namespace Video {
/// A directional light used for lighting the scene.
struct DirectionalLight {
    /// Direction.
    glm::vec4 direction;

    /// Intensity and ambient coefficient.
    glm::vec4 intensitiesAmbientCoefficient;
};

/// A light used for lighting the scene.
struct Light {
    /// Position and distance.
    glm::vec4 positionDistance;

    /// Intensity and attenuation.
    glm::vec4 intensitiesAttenuation;

    /// Direction and angle of the cone (in degrees).
    glm::vec4 directionConeAngle;
};
} // namespace Video
