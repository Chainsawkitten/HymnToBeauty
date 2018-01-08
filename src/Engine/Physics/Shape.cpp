#include <btBulletCollisionCommon.h>
#include "GlmConversion.hpp"
#include "Shape.hpp"

namespace Physics {
    Shape::Shape(const Shape::Sphere& params) {
        // Collision shapes are only used to determine collisions and have no
        // concept of physical quantities such as mass or inertia. Note that a
        // shape may be reused for multiple bodies.
        shape.reset(new btSphereShape(static_cast<btScalar>(params.radius)));
        kind = Kind::Sphere;
        sphere = params;
    }

    Shape::Shape(const Shape::Plane& params) {
        shape.reset(new btStaticPlaneShape(glmToBt(params.normal), static_cast<btScalar>(params.planeCoeff)));
        kind = Kind::Plane;
        plane = params;
    }

    Shape::Shape(const Shape::Box& params) {
        shape.reset(new btBoxShape(btVector3(params.width * 0.5f, params.height * 0.5f, params.depth * 0.5f)));
        kind = Kind::Box;
        box = params;
    }

    Shape::Shape(const Shape::Cylinder& params) {
        // From the Bullet source code:
        // cylinder is defined as following :
        //   - principle axis aligned along y by default, radius in x, z - value not used
        //   - for btCylinderShapeX : principle axis aligned along x, radius in y direction, z - value not used
        //   - for btCylinderShapeZ : principle axis aligned along z, radius in x direction, y - value not used
        shape.reset(new btCylinderShape(btVector3(params.radius, params.length * 0.5f, 0.0f)));
        kind = Kind::Cylinder;
        cylinder = params;
    }

    Shape::Shape(const Shape::Cone& params) {
        shape.reset(new btConeShape(params.radius, params.height));
        kind = Kind::Cone;
        cone = params;
    }

    Shape::Shape(const Shape::Capsule& params) {
        shape.reset(new btCapsuleShape(params.radius, params.height));
        kind = Kind::Capsule;
        capsule = params;
    }

    Shape::~Shape() {}

    Shape::Kind Shape::GetKind() const {
        return kind;
    }

    const Shape::Sphere* Shape::GetSphereData() const {
        return kind == Kind::Sphere ? &sphere : nullptr;
    }

    const Shape::Plane* Shape::GetPlaneData() const {
        return kind == Kind::Plane ? &plane : nullptr;
    }

    const Shape::Box* Shape::GetBoxData() const {
        return kind == Kind::Box ? &box : nullptr;
    }

    const Shape::Cylinder* Shape::GetCylinderData() const {
        return kind == Kind::Cylinder ? &cylinder : nullptr;
    }

    const Shape::Cone* Shape::GetConeData() const {
        return kind == Kind::Cone ? &cone : nullptr;
    }

    const Shape::Capsule* Shape::GetCapsuleData() const {
        return kind == Kind::Capsule ? &capsule : nullptr;
    }

    btCollisionShape* Shape::GetShape() const {
        return shape.get();
    }
}
