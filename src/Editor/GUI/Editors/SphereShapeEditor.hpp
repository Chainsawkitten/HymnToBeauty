#pragma once

#include "IShapeEditor.hpp"

namespace Component {
    class Shape;
}

namespace GUI {
    /// Sphere shape editor for physics components.
    class SphereShapeEditor : public IShapeEditor {
        public:
            const char* Label() const override { return "Sphere"; }
            void Show(Component::Shape* comp) override;
            void Apply(Component::Shape* comp) override;
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float radius = 1.0f;
    };
}
