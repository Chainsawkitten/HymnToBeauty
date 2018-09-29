#pragma once

#include "IShapeEditor.hpp"

namespace Component {
    class Shape;
}

namespace GUI {
    /// Plane shape editor for physics components.
    class PlaneShapeEditor : public IShapeEditor {
        public:
            /// Constructor
            PlaneShapeEditor() = default;

            const char* Label() const override { return "Plane"; }
            void Show(Component::Shape* comp) override;
            void Apply(Component::Shape* comp) override;
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float normal[3] = { 0.0f, 1.0f, 0.0f };
            float planeCoeff = 0.0f;
    };
}
