#include "Mesh.hpp"

#include "../Geometry/OBJModel.hpp"
#include "../Hymn.hpp"

using namespace Component;

Mesh::Mesh(Entity* entity) : SuperComponent(entity) {
    
}

Json::Value Mesh::Save() const {
    Json::Value component;
    
    Geometry::OBJModel* model = dynamic_cast<Geometry::OBJModel*>(geometry);
    if (model != nullptr)
        component["model"] = model->name;
    
    return component;
}

void Mesh::Load(const Json::Value& node) {
    std::string meshName = node.get("model", "").asString();
    for (Geometry::OBJModel* model : Hymn().models) {
        if (model->name == meshName)
            geometry = model;
    }
}
