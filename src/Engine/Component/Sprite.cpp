#include "Sprite.hpp"

#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Texture/TextureAsset.hpp"
#include "../Util/Json.hpp"

using namespace Component;

Sprite::Sprite() {
    texture = Managers().resourceManager->GetDefaultAlbedo();
}

Sprite::~Sprite() {
    if (texture != nullptr && texture != Managers().resourceManager->GetDefaultAlbedo())
        Managers().resourceManager->FreeTextureAsset(texture);
}

Json::Value Sprite::Save() const {
    Json::Value component;

    if (texture != nullptr)
        component["texture"] = texture->path + texture->name;

    component["pixelsPerUnit"] = pixelsPerUnit;
    component["pivot"] = Json::SaveVec2(pivot);
    component["tint"] = Json::SaveVec3(tint);
    component["alpha"] = alpha;
    component["layerMask"] = layerMask;

    return component;
}
