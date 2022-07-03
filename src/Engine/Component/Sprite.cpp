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

void Sprite::Serialize(Json::Value& node, bool load) {
    Json::Serialize(node, load, "texture", texture);
    Json::Serialize(node, load, "pixelsPerUnit", pixelsPerUnit, 100.0f);
    Json::Serialize(node, load, "pivot", pivot, glm::vec2(0.5, 0.5));
    Json::Serialize(node, load, "tint", tint, glm::vec3(1.0f, 1.0f, 1.0f));
    Json::Serialize(node, load, "alpha", alpha, 1.0f);
    Json::Serialize(node, load, "layerMask", layerMask, 1u);
}
