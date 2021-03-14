#include "TextureAsset.hpp"

#include "../Hymn.hpp"
#include "../Util/FileSystem.hpp"
#include <DefaultAlbedo.png.hpp>
#include <Video/Texture/Texture2D.hpp>
#include "../Manager/Managers.hpp"
#include "../Manager/RenderManager.hpp"

using namespace Video;

TextureAsset::TextureAsset(LowLevelRenderer* lowLevelRenderer) {
    this->lowLevelRenderer = lowLevelRenderer;
    texture = new Texture2D(lowLevelRenderer, DEFAULTALBEDO_PNG, DEFAULTALBEDO_PNG_LENGTH);
}

TextureAsset::TextureAsset(LowLevelRenderer* lowLevelRenderer, const char* source, int sourceLength) {
    texture = new Texture2D(lowLevelRenderer, source, sourceLength);
}

TextureAsset::~TextureAsset() {
    delete texture;
}

void TextureAsset::Save() const {}

void TextureAsset::Load(const std::string& name) {
    std::size_t pos = name.find_last_of('/');
    this->name = name.substr(pos + 1);
    path = name.substr(0, pos + 1);
    std::string filename = Hymn().GetPath() + "/" + name;

    if (FileSystem::FileExists(filename.c_str())) {
        // Delete old texture.
        if (texture != nullptr)
            delete texture;

        // Load texture from disk.
        texture = new Texture2D(lowLevelRenderer, filename.c_str());
    }
}

Texture2D* TextureAsset::GetTexture() const {
    return texture;
}
