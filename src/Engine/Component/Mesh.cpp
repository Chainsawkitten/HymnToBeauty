#include "Mesh.hpp"

#include "../Geometry/Model.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Util/Json.hpp"

using namespace Component;

Mesh::Mesh() {}

Mesh::~Mesh() {
    if (model != nullptr)
        Managers().resourceManager->FreeModel(model);
}

void Mesh::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "model", model);
    Json::Serialize(node, load, "layerMask", layerMask, 1u);
}
