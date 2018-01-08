#pragma once

#include "SuperComponent.hpp"

namespace Component {
    /// %Component describing a listener that can listen to sounds.
    class Listener : public SuperComponent {
        public:
            /// Create new listener.
            Listener();
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;
    };
}
