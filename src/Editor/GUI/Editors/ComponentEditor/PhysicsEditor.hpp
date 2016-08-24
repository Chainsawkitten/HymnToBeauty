#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;

namespace GUI {
    class Vec3Editor;
    class FloatEditor;
    
    /// Used to edit a physics component.
    class PhysicsEditor : public ComponentEditor {
        public:
            /// Create new physics editor.
            /**
             * @param parent Parent widget.
             */
            PhysicsEditor(Widget* parent);
            
            /// Destructor.
            ~PhysicsEditor() override;
            
            /// Set the entity to edit the physics component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            Font* font;
            
            Vec3Editor* velocityEditor;
            FloatEditor* maxVelocityEditor;
            Vec3Editor* angularVelocityEditor;
            FloatEditor* maxAngularVelocityEditor;
            Vec3Editor* accelerationEditor;
            Vec3Editor* angularAccelerationEditor;
            FloatEditor* velocityDragFactorEditor;
            FloatEditor* angularDragFactorEditor;
            FloatEditor* gravityFactorEditor;
            Vec3Editor* momentOfInertiaEditor;
    };
}
