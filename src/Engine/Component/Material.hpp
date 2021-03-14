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

    Json::Value Save() const override;

    /// Albedo texture.
    TextureAsset* albedo;

    /// Normal texture.
    TextureAsset* normal;

    /// Roughness-metallic texture.
    /**
     * Roughness in green channel. Metallic in blue channel.
     */
    TextureAsset* roughnessMetallic;

  private:
    void LoadTexture(TextureAsset*& texture, const std::string& name);
};
} // namespace Component
