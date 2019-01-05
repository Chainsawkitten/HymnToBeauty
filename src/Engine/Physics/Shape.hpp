#pragma once

#include <glm/vec3.hpp>
#include <memory>

class btCollisionShape;
class PhysicsManager;

namespace Component {
class RigidBody;
}

namespace Physics {
class Trigger;

/// Represents a shape for physics objects and facilitates creation of
/// underlying types.
class Shape {
    friend class ::PhysicsManager;
    friend class ::Component::RigidBody;
    friend class Trigger;

  public:
    /// Parameters used to create a sphere shape.
    struct Sphere {
        explicit Sphere(float radius) : radius(radius) {}
        float radius;
    };

    /// Parameters used to create a plane shape.
    struct Plane {
        Plane(const glm::vec3& normal, float planeCoeff) : normal(normal), planeCoeff(planeCoeff) {}
        glm::vec3 normal;
        float planeCoeff;
    };

    /// Parameters used to create a box shape.
    struct Box {
        Box(float width, float height, float depth) : width(width), height(height), depth(depth) {}
        float width;
        float height;
        float depth;
    };

    /// Parameters used to create a cylinder shape.
    struct Cylinder {
        Cylinder(float radius, float length) : radius(radius), length(length) {}
        float radius;
        float length;
    };

    /// Parameters used to create a cone shape.
    struct Cone {
        Cone(float radius, float height) : radius(radius), height(height) {}
        float radius;
        float height;
    };

    /// Parameters used to create a capsule shape.
    struct Capsule {
        Capsule(float radius, float height) : radius(radius), height(height) {}
        float radius;
        float height;
    };

    /// The various kinds of shapes that are wrapped by %Shape.
    enum class Kind {
        Sphere,
        Plane,
        Box,
        Cylinder,
        Cone,
        Capsule,
    };

    /// Construct a sphere shape.
    /**
     * @param params Sphere specific parameters.
     */
    explicit Shape(const Sphere& params);

    /// Construct a plane shape.
    /**
     * @param params Plane specific parameters.
     */
    explicit Shape(const Plane& params);

    /// Construct a box shape.
    /**
     * @param params Box specific parameters.
     */
    explicit Shape(const Box& params);

    /// Construct a cylinder shape.
    /**
     * @param params Cylinder specific parameters.
     */
    explicit Shape(const Cylinder& params);

    /// Construct a cone shape.
    /**
     * @param params Cone specific parameters.
     */
    explicit Shape(const Cone& params);

    /// Construct a capsule shape.
    /**
     * @param params Capsule specific parameters.
     */
    explicit Shape(const Capsule& params);

    /// Destructor
    ~Shape();

    /// Get the type of wrapped shape.
    /**
     * @return The type of shape.
     */
    Kind GetKind() const;

    /// Get sphere data of the shape.
    /**
     * @return Sphere data, or nullptr if the shape is not a sphere.
     */
    const Sphere* GetSphereData() const;

    /// Get plane data of the shape.
    /**
     * @return Plane data, or nullptr if the shape is not a plane.
     */
    const Plane* GetPlaneData() const;

    /// Get box data of the shape.
    /**
     * @return Box data, or nullptr if the shape is not a box.
     */
    const Box* GetBoxData() const;

    /// Get cylinder data of the shape.
    /**
     * @return Cylinder data, or nullptr if the shape is not a cylinder.
     */
    const Cylinder* GetCylinderData() const;

    /// Get cone data of the shape.
    /**
     * @return Cone data, or nullptr if the shape is not a cone.
     */
    const Cone* GetConeData() const;

    /// Get capsule data of the shape.
    /**
     * @return Capsule data, or nullptr if the shape is not a capsule.
     */
    const Capsule* GetCapsuleData() const;

  private:
    /// Get the wrapped Bullet shape.
    /**
     * @return The Bullet shape.
     */
    btCollisionShape* GetShape() const;

    Shape(const Shape& other) = delete;

    std::unique_ptr<btCollisionShape> shape;
    Kind kind;
    union {
        Sphere sphere;
        Plane plane;
        Box box;
        Cylinder cylinder;
        Cone cone;
        Capsule capsule;
    };
};
} // namespace Physics
