#pragma once

#include <glm/glm.hpp>

class Entity;

/// Use mouse cursor to select objects in editor and in game.
class MousePicking {
  public:
    /// Fake constructor.
    MousePicking() {}

    /// Constructor.
    /**
     * @param cam The camera the mouse picking makes calculations from
     * @param projection The camera's projection matrix.
     */
    MousePicking(Entity* cam, const glm::mat4& projection);

    /// Destructor.
    ~MousePicking();

    /// Return the vector for current ray.
    /**
     * @return A vec3 with values from current ray in world space. Normalized.
     */
    glm::vec3 GetCurrentRay() const;

    /// Updates the mouse picker's projection matrix
    /**
     * @param projection The projection matrix to update with.
     */
    void UpdateProjectionMatrix(const glm::mat4& projection);

    /// Update the view matrix and recalculates the ray.
    void Update();

  private:
    // Where current ray direction points in world space.
    glm::vec3 currentRay;

    // Projection matrix.
    glm::mat4 pMatrix;

    // View matrix.
    glm::mat4 vMatrix;

    // Entity to hold camera.
    Entity* camera;

    // Return a vector
    glm::vec3 CalculateRay() const;

    // Return a vector with mouse coordinates in Normalized Device Coordinates.
    static glm::vec2 GetNDC(double mouseX, double mouseY);

    // Convert clip space coordinates to eye coordinates.
    glm::vec4 ConvertEyeCoords(const glm::vec4& clipSpaceCoordinates) const;

    // Convert eye coordinates to world coordinates.
    glm::vec3 ConvertWorldCoords(const glm::vec4& eyeCoords) const;
};
