#pragma once

#include "IShapeEditor.hpp"

namespace Component {
    class Shape;
}

namespace GUI {
    /// Cone shape editor for physics components.
    class ConeShapeEditor : public IShapeEditor {
        public:
            /// Constructor
            ConeShapeEditor() = default;

            const char* Label() const override { return "Cone"; }
            void Show(Component::Shape* comp) override;
            void Apply(Component::Shape* comp) override;
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float radius = 1.0f;
            float height = 1.0f;
    };
}
