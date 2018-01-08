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

            /// Box type label.
            /**
             * @return "Box" string.
             */
            const char* Label() const override { return "Box"; }

            /// Show controls for editing a box shape.
            /**
             * @param comp The shape component to edit.
             */
            void Show(Component::Shape* comp) override;

            /// Set a box shape on the given shape component.
            /**
             * @param comp The shape component on which to set shape.
             */
            void Apply(Component::Shape* comp) override;

            /// Set internal data according the given %Shape, provided that it
            /// is a box.
            /**
             * @param shape The %Shape from which to initialize data.
             * @return True if shape type is box, false otherwise.
             */
            bool SetFromShape(const Physics::Shape& shape) override;

        private:
            float width = 1.0f;
            float height = 1.0f;
            float depth = 1.0f;
    };
}
