#pragma once

#include "SuperComponent.hpp"

namespace Geometry {
class Model;
}

namespace Component {
/// %Component providing geometry to an entity.
class Mesh : public SuperComponent {
  public:
    /// Create new mesh.
    Mesh();

    /// Destructor.
    ~Mesh() override;

    void Serialize(Json::Value& node, bool load) override;

    /// Geometry data.
    /**
     * Default: nullptr
     */
    Geometry::Model* model = nullptr;

    /// Bitmask of which layers to render to.
    uint32_t layerMask = 1u;
};
} // namespace Component
