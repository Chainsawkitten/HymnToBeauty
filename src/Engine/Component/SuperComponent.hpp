#pragma once

#include <json/json.h>

class Entity;

namespace Component {
    /// %Component which all components inherit.
    class SuperComponent {
        public:
            /// Create new %SuperComponent.
            /**
             * @param entity Pointer to which entity this component corresponds.
             */
            SuperComponent(Entity* entity);
            
            /// Destructor.
            virtual ~SuperComponent();
            
            /// Save the component.
            /**
             * @return JSON value to be stored on disk.
             */
            virtual Json::Value Save() const;
            
            /// Load component from JSON node.
            /**
             * @param node JSON node to load from.
             */
            virtual void Load(const Json::Value& node);
            
            /// Pointer to which entity this component corresponds.
            Entity* entity;
    };
}
