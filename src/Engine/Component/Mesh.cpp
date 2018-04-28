#include "Mesh.hpp"

#include "../Geometry/Model.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"

using namespace Component;

Mesh::Mesh() {

}

Mesh::~Mesh() {
    Geometry::Model* model = dynamic_cast<Geometry::Model*>(geometry);
    if (model != nullptr)
        Managers().resourceManager->FreeModel(model);
}

Json::Value Mesh::Save() const {
    Json::Value component;

    Geometry::Model* model = dynamic_cast<Geometry::Model*>(geometry);
    if (model != nullptr)
        component["model"] = model->path + model->name;

    return component;
}
