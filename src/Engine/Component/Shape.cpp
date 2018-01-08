#include <Utility/Log.hpp>
#include "../Physics/Shape.hpp"
#include "../Util/Json.hpp"
#include "Shape.hpp"

namespace Component {
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
                concreteShape["normal"] = Json::SaveVec3(planeData->normal);
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

    std::shared_ptr<Physics::Shape> Shape::GetShape() const {
        return shape;
    }

    void Shape::SetShape(std::shared_ptr<Physics::Shape> shape) {
        this->shape = shape;
    }
}
