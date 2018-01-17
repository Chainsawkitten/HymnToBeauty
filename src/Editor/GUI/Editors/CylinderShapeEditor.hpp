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

            /// Cylinder type label.
            /**
             * @return "Cylinder" string.
             */
            const char* Label() const override { return "Cylinder"; }

            /// Show controls for editing a cylinder shape.
            /**
             * @param comp The shape component to edit.
             */
            void Show(Component::Shape* comp) override;

            /// Set a cylinder shape on the given shape component.
            /**
             * @param comp The shape component on which to set shape.
             */
            void Apply(Component::Shape* comp) override;

            /// Set internal data according the given %Shape, provided that it
            /// is a cylinder.
            /**
             * @param shape The %Shape from which to initialize data.
             * @return True if shape type is cylinder, false otherwise.
             */
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float radius = 1.0f;
            float length = 1.0f;
    };
}
