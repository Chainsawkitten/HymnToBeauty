#pragma once

#include "SuperComponent.hpp"

class Entity;

namespace Component {
    /// %Component describing a listener that can listen to sounds.
    class Listener : public SuperComponent {
        public:
            /// Create new listener.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            Listener(Entity* entity);
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const override;
            
            /// Load component from JSON node.
            /**
             * @param node JSON node to load from.
             */
            void Load(const Json::Value& node) override;
            
            /// Whether to apply a lowpass filter to head sounds.
            bool lowpass = false;
    };
}
