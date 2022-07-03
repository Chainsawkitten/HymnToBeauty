#pragma once

#include "SuperComponent.hpp"

class TextureAsset;

namespace Component {
/// %Component providing a material to Entity.
class Material : public SuperComponent {
  public:
    /// Create new material.
    Material();

    /// Destructor.
    ~Material() override;

    void Serialize(Json::Value& node, bool load) override;

    /// Albedo texture.
    TextureAsset* albedo;

    /// Normal texture.
    TextureAsset* normal;

    /// Roughness-metallic texture.
    /**
     * Roughness in green channel. Metallic in blue channel.
     */
    TextureAsset* roughnessMetallic;
};
} // namespace Component
