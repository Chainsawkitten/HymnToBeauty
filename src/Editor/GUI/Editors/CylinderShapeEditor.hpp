#pragma once

#include "IShapeEditor.hpp"

namespace Component {
class Shape;
}

namespace GUI {
/// Cylinder shape editor for physics components.
class CylinderShapeEditor : public IShapeEditor {
  public:
    /// Constructor
    CylinderShapeEditor() = default;

    const char* Label() const override {
        return "Cylinder";
    }
    void Show(Component::Shape* comp) override;
    void Apply(Component::Shape* comp) override;
    bool SetFromShape(const Physics::Shape& shape) override;

  private:
    float radius = 1.0f;
    float length = 1.0f;
};
} // namespace GUI
