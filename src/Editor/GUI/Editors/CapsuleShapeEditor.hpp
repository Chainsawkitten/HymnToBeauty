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

            /// Capsule type label.
            /**
             * @return "Capsule" string.
             */
            const char* Label() const override { return "Capsule"; }

            /// Show controls for editing a capsule shape.
            /**
             * @param comp The shape component to edit.
             */
            void Show(Component::Shape* comp) override;

            /// Set a capsule shape on the given shape component.
            /**
             * @param comp The shape component on which to set shape.
             */
            void Apply(Component::Shape* comp) override;

            /// Set internal data according the given %Shape, provided that it
            /// is a capsule.
            /**
             * @param shape The %Shape from which to initialize data.
             * @return True if shape type is capsule, false otherwise.
             */
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float radius = 1.0f;
            float height = 1.0f;
    };
}
