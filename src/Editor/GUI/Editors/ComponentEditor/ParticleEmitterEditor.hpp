#pragma once

#include "ComponentEditor.hpp"

class Entity;
class Font;

namespace GUI {
    class BoolEditor;
    class DoubleEditor;
    class FloatEditor;
    class IntEditor;
    class Vec2Editor;
    class Vec3Editor;
    
    /// Used to edit a particle emitter component.
    class ParticleEmitterEditor : public ComponentEditor {
        public:
            /// Create new particle emitter editor.
            /**
             * @param parent Parent widget.
             */
            ParticleEmitterEditor(Widget* parent);
            
            /// Destructor.
            ~ParticleEmitterEditor() override;
            
            /// Set the entity to edit the particle emitter component of.
            /**
             * @param entity %Entity to edit.
             */
            void SetEntity(Entity* entity) override;
            
        private:
            Font* font;
            
            IntEditor* textureIndexEditor;
            Vec3Editor* minVelocityEditor;
            Vec3Editor* maxVelocityEditor;
            FloatEditor* minLifetimeEditor;
            FloatEditor* maxLifetimeEditor;
            Vec2Editor* minSizeEditor;
            Vec2Editor* maxSizeEditor;
            BoolEditor* uniformScalingEditor;
            FloatEditor* startAlphaEditor;
            FloatEditor* midAlphaEditor;
            FloatEditor* endAlphaEditor;
            Vec3Editor* colorEditor;
            Vec3Editor* sizeEditor;
            DoubleEditor* minEmitTimeEditor;
            DoubleEditor* maxEmitTimeEditor;
            //IntEditor* emitterTypeEditor;
    };
}
