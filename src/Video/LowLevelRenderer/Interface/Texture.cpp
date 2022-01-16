#include "Texture.hpp"

namespace Video {

// 0 is reserved for null.
static uint64_t textureCount = 1u;

Texture::Texture(Type type, const glm::uvec2& size) {
    this->type = type;
    this->size = size;
    uniqueIdentifier = textureCount++;
}

uint64_t Texture::GetUniqueIdentifier() const {
    return uniqueIdentifier;
}

}