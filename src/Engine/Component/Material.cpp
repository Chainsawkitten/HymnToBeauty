#include "Material.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Hymn.hpp"

using namespace Component;

Material::Material() {
    albedo = Managers().resourceManager->GetDefaultAlbedo();
    normal = Managers().resourceManager->GetDefaultNormal();
    roughnessMetallic = Managers().resourceManager->GetDefaultRoughnessMetallic();
}

Material::~Material() {
    if (albedo != nullptr && albedo != Managers().resourceManager->GetDefaultAlbedo())
        Managers().resourceManager->FreeTextureAsset(albedo);

    if (normal != nullptr && normal != Managers().resourceManager->GetDefaultNormal())
        Managers().resourceManager->FreeTextureAsset(normal);

    if (roughnessMetallic != nullptr && roughnessMetallic != Managers().resourceManager->GetDefaultRoughnessMetallic())
        Managers().resourceManager->FreeTextureAsset(roughnessMetallic);
}

Json::Value Material::Save() const {
    Json::Value component;

    if (albedo != nullptr)
        component["albedo"] = albedo->path + albedo->name;

    if (normal != nullptr)
        component["normal"] = normal->path + normal->name;

    if (roughnessMetallic != nullptr)
        component["roughnessMetallic"] = roughnessMetallic->path + roughnessMetallic->name;

    return component;
}
