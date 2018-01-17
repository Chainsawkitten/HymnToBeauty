#include "Material.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Hymn.hpp"

using namespace Component;

Material::Material() {
    albedo = Hymn().defaultAlbedo;
    normal = Hymn().defaultNormal;
    metallic = Hymn().defaultMetallic;
    roughness = Hymn().defaultRoughness;
}

Material::~Material() {
    if (albedo != nullptr && albedo != Hymn().defaultAlbedo)
        Managers().resourceManager->FreeTextureAsset(albedo);
    
    if (normal != nullptr && normal != Hymn().defaultNormal)
        Managers().resourceManager->FreeTextureAsset(normal);
    
    if (metallic != nullptr && metallic != Hymn().defaultMetallic)
        Managers().resourceManager->FreeTextureAsset(metallic);
    
    if (roughness != nullptr && roughness != Hymn().defaultRoughness)
        Managers().resourceManager->FreeTextureAsset(roughness);
}

Json::Value Material::Save() const {
    Json::Value component;
    
    if (albedo != nullptr)
        component["albedo"] = albedo->path + albedo->name;
    
    if (normal != nullptr)
        component["normal"] = normal->path + normal->name;
    
    if (metallic != nullptr)
        component["metallic"] = metallic->path + metallic->name;
    
    if (roughness != nullptr)
        component["roughness"] = roughness->path + roughness->name;
    
    return component;
}

void Material::LoadTexture(TextureAsset*& texture, const std::string& name) {
    if (!name.empty())
        texture = Managers().resourceManager->CreateTextureAsset(name);
}
