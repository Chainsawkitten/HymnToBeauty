#pragma once

#include "SuperComponent.hpp"

#include <glm/glm.hpp>

class TextureAsset;

namespace Component {
/// %Component defining a 2D sprite.
class Sprite : public SuperComponent {
  public:
    /// Create new sprite.
    Sprite();

    /// Destructor.
    ~Sprite() override;

    Json::Value Save() const override;

    /// Texture.
    TextureAsset* texture;

    /// How many pixels go into one world unit.
    float pixelsPerUnit = 100.0f;

    /// The point around which the sprite should be rotated.
    glm::vec2 pivot = glm::vec2(0.5, 0.5);

    /// A color to tint the sprite with.
    glm::vec3 tint = glm::vec3(1.0f, 1.0f, 1.0f);

    /// The opacity of the sprite (multiplied with the texture's alpha channel).
    float alpha = 1.0f;
};
} // namespace Component
