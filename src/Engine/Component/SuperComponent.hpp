#pragma once

#include <json/json.h>

class Entity;

namespace Component {
    /// %Component which all components inherit.
    class SuperComponent {
        public:
            /// Create new %SuperComponent.
            SuperComponent();

            /// Destructor.
            virtual ~SuperComponent();

            /// Kill the component, will be removed at the end of the frame.
            void Kill();

            /// Get whether component has been killed.
            /**
             * @return Whether component has been killed.
             */
            bool IsKilled() const;

            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            virtual Json::Value Save() const;

            /// Pointer to which entity this component corresponds.
            Entity* entity = nullptr;

        private:
            bool killed = false;
    };
}
