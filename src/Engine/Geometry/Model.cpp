#include "Model.hpp"

#include <glm/gtc/quaternion.hpp>
#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"

using namespace Geometry;

Assimp::Importer Model::aImporter = Assimp::Importer();

Model::Model() {
    
}

Model::~Model() {
    
}

Json::Value Model::Save() const {
    Json::Value model;
    model["name"] = name;
    model["extension"] = extension;
    model["type"] = GetType() == STATIC ? "Static" : "Skin";
    return model;
}

void Model::Load(const Json::Value& node) {
    name = node.get("name", "").asString();
    extension = node.get("extension", "").asString();
    Load((Hymn().GetPath() + FileSystem::DELIMITER + "Models" + FileSystem::DELIMITER + name + "." + extension).c_str());
}
