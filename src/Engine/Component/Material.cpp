#include "Material.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Hymn.hpp"

using namespace Component;

Material::Material() {
    albedo = Managers().resourceManager->GetDefaultAlbedo();
    normal = Managers().resourceManager->GetDefaultNormal();
    metallic = Managers().resourceManager->GetDefaultMetallic();
    roughness = Managers().resourceManager->GetDefaultRoughness();
}

Material::~Material() {
    if (albedo != nullptr && albedo != Managers().resourceManager->GetDefaultAlbedo())
        Managers().resourceManager->FreeTextureAsset(albedo);

    if (normal != nullptr && normal != Managers().resourceManager->GetDefaultNormal())
        Managers().resourceManager->FreeTextureAsset(normal);

    if (metallic != nullptr && metallic != Managers().resourceManager->GetDefaultMetallic())
        Managers().resourceManager->FreeTextureAsset(metallic);

    if (roughness != nullptr && roughness != Managers().resourceManager->GetDefaultRoughness())
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
