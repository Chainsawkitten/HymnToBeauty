#include "Material.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Hymn.hpp"
#include "../Util/Json.hpp"

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

void Material::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "albedo", albedo);
    Json::Serialize(node, load, "normal", normal);
    Json::Serialize(node, load, "roughnessMetallic", roughnessMetallic);
}
