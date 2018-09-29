#pragma once

#include "IShapeEditor.hpp"

namespace Component {
    class Shape;
}

namespace GUI {
    /// Box shape editor for physics components.
    class BoxShapeEditor : public IShapeEditor {
        public:
            /// Constructor
            BoxShapeEditor() = default;

            const char* Label() const override { return "Box"; }
            void Show(Component::Shape* comp) override;
            void Apply(Component::Shape* comp) override;
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float width = 1.0f;
            float height = 1.0f;
            float depth = 1.0f;
    };
}
