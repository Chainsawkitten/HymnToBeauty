#pragma once

#include "IShapeEditor.hpp"

namespace Component {
    class Shape;
}

namespace GUI {
    /// Capsule shape editor for physics components.
    class CapsuleShapeEditor : public IShapeEditor {
        public:
            /// Constructor
            CapsuleShapeEditor() = default;

            const char* Label() const override { return "Capsule"; }
            void Show(Component::Shape* comp) override;
            void Apply(Component::Shape* comp) override;
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float radius = 1.0f;
            float height = 1.0f;
    };
}
