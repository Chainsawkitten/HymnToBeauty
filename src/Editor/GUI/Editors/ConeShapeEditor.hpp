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

            /// Cone type label.
            /**
             * @return "Cone" string.
             */
            const char* Label() const override { return "Cone"; }

            /// Show controls for editing a cone shape.
            /**
             * @param comp The shape component to edit.
             */
            void Show(Component::Shape* comp) override;

            /// Set a cone shape on the given shape component.
            /**
             * @param comp The shape component on which to set shape.
             */
            void Apply(Component::Shape* comp) override;

            /// Set internal data according the given %Shape, provided that it
            /// is a cone.
            /**
             * @param shape The %Shape from which to initialize data.
             * @return True if shape type is cone, false otherwise.
             */
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float radius = 1.0f;
            float height = 1.0f;
    };
}
