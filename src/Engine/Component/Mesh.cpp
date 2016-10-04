#include "Mesh.hpp"

#include "../Geometry/Model.hpp"
#include "../Hymn.hpp"

using namespace Component;

Mesh::Mesh(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Mesh::Save() const {
    Json::Value component;
    
    Geometry::Model* model = dynamic_cast<Geometry::Model*>(geometry);
    if (model != nullptr)
        component["model"] = model->name;
    
    return component;
}

void Mesh::Load(const Json::Value& node) {
    std::string meshName = node.get("model", "").asString();
    for (Geometry::Model* model : Hymn().models) {
        if (model->name == meshName)
            geometry = model;
    }
}
