#include "Texture2D.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include <stb_image.h>

#include "../LowLevelRenderer/Interface/LowLevelRenderer.hpp"

#include <Utility/Log.hpp>

using namespace Video;

Texture2D::Texture2D() {}

Texture2D::Texture2D(LowLevelRenderer* lowLevelRenderer, const char* filename) {
    // Load texture from file.
    int components, width, height;
    unsigned char* data = stbi_load(filename, &width, &height, &components, 0);

    if (data == NULL)
        Log() << "Couldn't load image file " << filename << ".\n";

    // Create texture.
    texture = lowLevelRenderer->CreateTexture(glm::uvec2(width, height), Texture::Format::R8G8B8A8, components, data);

    // We no longer need the CPU side data.
    stbi_image_free(data);

    loaded = true;
}

Texture2D::Texture2D(LowLevelRenderer* lowLevelRenderer, const char* source, int sourceLength) {
    // Load texture from memory.
    int components, width, height;
    unsigned char* data = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(source), sourceLength, &width, &height, &components, 0);

    if (data == NULL)
        Log() << "Couldn't load headerized image.\n";

    // Create texture.
    texture = lowLevelRenderer->CreateTexture(glm::uvec2(width, height), Texture::Format::R8G8B8A8, components, data);

    // We no longer need the CPU side data.
    stbi_image_free(data);

    loaded = true;
}

Texture2D::Texture2D(LowLevelRenderer* lowLevelRenderer, const glm::uvec2& size, const glm::vec4& color) {
    // Copy the specified value to all pixels.
    unsigned char value[4];
    value[0] = static_cast<unsigned char>(color.r * 255.0f);
    value[1] = static_cast<unsigned char>(color.g * 255.0f);
    value[2] = static_cast<unsigned char>(color.b * 255.0f);
    value[3] = static_cast<unsigned char>(color.a * 255.0f);

    unsigned char* data = new unsigned char[4 * size.x * size.y];
    for (unsigned int i = 0; i < 4 * size.x * size.y; ++i) {
        data[i] = value[i % 4];
    }

    texture = lowLevelRenderer->CreateTexture(size, Texture::Format::R8G8B8A8, 4, data);

    delete[] data;

    loaded = true;
}

Texture2D::~Texture2D() {
    if (texture != nullptr)
        delete texture;
}

Texture* Texture2D::GetTexture() {
    return texture;
}

bool Texture2D::IsLoaded() const {
    return loaded;
}

