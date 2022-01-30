#include "Texture.hpp"

namespace Video {

// 0 is reserved for null.
static uint64_t textureCount = 1u;

Texture::Texture(Type type, const glm::uvec2& size, Format format) {
    this->type = type;
    this->size = size;
    this->format = format;
    uniqueIdentifier = textureCount++;
}

uint64_t Texture::GetUniqueIdentifier() const {
    return uniqueIdentifier;
}

}