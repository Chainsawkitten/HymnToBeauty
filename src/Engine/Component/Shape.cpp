#include "Shape.hpp"

#include <btBulletDynamicsCommon.h>
#include <Utility/Log.hpp>
#include "../Physics/Shape.hpp"
#include "../Util/Json.hpp"
#include "../Entity/Entity.hpp"
#include "RigidBody.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/PhysicsManager.hpp"

namespace Component {

void Shape::Serialize(Json::Value& node, bool load) {
    if (load) {
        Load(node);
    } else {
        node = Save();
    }
}

std::shared_ptr<Physics::Shape> Shape::GetShape() const {
    return shape;
}

void Shape::SetShape(std::shared_ptr<Physics::Shape> shape) {
    this->shape = shape;
}

Json::Value Shape::Save() const {
    Json::Value component;

    Json::Value concreteShape;
    switch (shape->GetKind()) {
    case Physics::Shape::Kind::Sphere: {
        auto sphereData = shape->GetSphereData();
        concreteShape["radius"] = sphereData->radius;
        component["sphere"] = concreteShape;
        break;
    }
    case Physics::Shape::Kind::Plane: {
        auto planeData = shape->GetPlaneData();
        glm::vec3 normal = planeData->normal;
        Json::Serialize(concreteShape, false, "normal", normal, glm::vec3(0.0f, 0.0f, 0.0f));
        concreteShape["planeCoeff"] = planeData->planeCoeff;
        component["plane"] = concreteShape;
        break;
    }
    case Physics::Shape::Kind::Box: {
        auto boxData = shape->GetBoxData();
        concreteShape["width"] = boxData->width;
        concreteShape["height"] = boxData->height;
        concreteShape["depth"] = boxData->depth;
        component["box"] = concreteShape;
        break;
    }
    case Physics::Shape::Kind::Cylinder: {
        auto cylinderData = shape->GetCylinderData();
        concreteShape["radius"] = cylinderData->radius;
        concreteShape["length"] = cylinderData->length;
        component["cylinder"] = concreteShape;
        break;
    }
    case Physics::Shape::Kind::Cone: {
        auto coneData = shape->GetConeData();
        concreteShape["radius"] = coneData->radius;
        concreteShape["height"] = coneData->height;
        component["cone"] = concreteShape;
        break;
    }
    case Physics::Shape::Kind::Capsule: {
        auto capsuleData = shape->GetCapsuleData();
        concreteShape["radius"] = capsuleData->radius;
        concreteShape["height"] = capsuleData->height;
        component["capsule"] = concreteShape;
        break;
    }
    default:
        Log(Log::ERR) << "Component::Shape::Save: Unsupported shape `" << static_cast<uint32_t>(shape->GetKind()) << "`.\n";
    }

    return component;
}

void Shape::Load(Json::Value& node) {
    if (node.isMember("sphere")) {
        auto sphere = node.get("sphere", {});
        auto radius = sphere.get("radius", 1.0f).asFloat();
        auto shape = std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Sphere(radius)));
        SetShape(shape);
    } else if (node.isMember("plane")) {
        auto plane = node.get("plane", {});
        glm::vec3 normal;
        Json::Serialize(plane, true, "normal", normal, glm::vec3(0.0f, 0.0f, 0.0f));
        auto planeCoeff = plane.get("planeCoeff", 0.0f).asFloat();
        auto shape = std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Plane(normal, planeCoeff)));
        SetShape(shape);
    } else if (node.isMember("box")) {
        auto box = node.get("box", {});
        auto width = box.get("width", 1.0f).asFloat();
        auto height = box.get("height", 1.0f).asFloat();
        auto depth = box.get("depth", 1.0f).asFloat();
        auto shape = std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Box(width, height, depth)));
        SetShape(shape);
    } else if (node.isMember("cylinder")) {
        auto cylinder = node.get("cylinder", {});
        auto radius = cylinder.get("radius", 1.0f).asFloat();
        auto length = cylinder.get("length", 1.0f).asFloat();
        auto shape = std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Cylinder(radius, length)));
        SetShape(shape);
    } else if (node.isMember("cone")) {
        auto cone = node.get("cone", {});
        auto radius = cone.get("radius", 1.0f).asFloat();
        auto height = cone.get("height", 1.0f).asFloat();
        auto shape = std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Cone(radius, height)));
        SetShape(shape);
    } else if (node.isMember("capsule")) {
        auto capsule = node.get("capsule", {});
        auto radius = capsule.get("radius", 1.0f).asFloat();
        auto height = capsule.get("height", 1.0f).asFloat();
        auto shape = std::shared_ptr<Physics::Shape>(new Physics::Shape(Physics::Shape::Capsule(radius, height)));
        SetShape(shape);
    }

    auto rigidBodyComp = entity->GetComponent<Component::RigidBody>();
    if (rigidBodyComp) {
        rigidBodyComp->SetCollisionShape(GetShape());
        rigidBodyComp->SetMass(rigidBodyComp->GetMass());
    }
}
} // namespace Component
