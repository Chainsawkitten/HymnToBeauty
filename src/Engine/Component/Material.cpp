#include "Material.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Texture/Texture2D.hpp"
#include "DefaultDiffuse.png.hpp"
#include "DefaultNormal.png.hpp"
#include "DefaultSpecular.png.hpp"
#include "DefaultGlow.png.hpp"
#include "../Hymn.hpp"

using namespace Component;

Material::Material(Entity* entity) : SuperComponent(entity) {
    diffuse = Hymn().defaultDiffuse;
    normal = Hymn().defaultNormal;
    specular = Hymn().defaultSpecular;
    glow = Hymn().defaultGlow;
}

Json::Value Material::Save() const {
    Json::Value component;
    
    if (diffuse != nullptr)
        component["diffuse"] = diffuse->name;
    
    if (normal != nullptr)
        component["normal"] = normal->name;
    
    if (specular != nullptr)
        component["specular"] = specular->name;
    
    if (glow != nullptr)
        component["glow"] = glow->name;
    
    return component;
}

void Material::Load(const Json::Value& node) {
    LoadTexture(diffuse, node.get("diffuse", "").asString());
    LoadTexture(normal, node.get("normal", "").asString());
    LoadTexture(specular, node.get("specular", "").asString());
    LoadTexture(glow, node.get("glow", "").asString());
}

void Material::LoadTexture(Texture2D*& texture, const std::string& name) {
    for (Texture2D* t : Hymn().textures) {
        if (t->name == name)
            texture = t;
    }
}
