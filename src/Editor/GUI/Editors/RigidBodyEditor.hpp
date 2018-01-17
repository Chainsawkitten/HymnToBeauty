#pragma once

namespace Component {
    class RigidBody;
}

namespace GUI {
    /// Editor for rigid body components.
    class RigidBodyEditor {
        public:
            /// Show controls for editing a rigid body.
            /**
             * @param comp Component to edit.
             */
            void Show(Component::RigidBody* comp);

        private:
            void GetData(Component::RigidBody* comp);

            float mass = 1.0f;
    };
}
