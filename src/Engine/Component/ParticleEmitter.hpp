#pragma once

#include "SuperComponent.hpp"
#include <glm/glm.hpp>

class Entity;

namespace Component {
    /// Emitter that emits particles.
    class ParticleEmitter : public SuperComponent {
        public:
            /// Type of emitter
            enum EmitterType {
                POINT = 0, ///< Point emitter
                CUBOID, ///< Cuboid emitter
            };
            
            /// Defines a kind of particle.
            struct ParticleType {
                /// Index on texture atlas to apply to particles.
                int textureIndex = 0;
                
                /// The minimum initial velocity in all directions.
                glm::vec3 minVelocity;
                
                /// The maximum initial velocity in all directions.
                glm::vec3 maxVelocity;
                
                /// The minimum lifetime of the particle (in seconds).
                float minLifetime = 0.f;
                
                /// The maximum lifetime of the particle (in seconds).
                float maxLifetime = 0.f;
                
                /// The minimum size of the particle.
                glm::vec2 minSize;
                
                /// The maximum size of the particle.
                glm::vec2 maxSize;
                
                /// Whether to scale all axes individually or uniformly.
                bool uniformScaling = false;
                
                /// Alpha at the beginning of the particle's life.
                float startAlpha = 0.f;
                
                /// Alpha in the middle of the particle's life.
                float midAlpha = 1.f;
                
                /// Alpha at the end of the particle's life.
                float endAlpha = 0.f;
                
                /// Blend color.
                glm::vec3 color = glm::vec3(1.f, 1.f, 1.f);
            };
            
            /// Create new particle emitter.
            /**
             * @param entity to which we attatch the emitter
             */
            ParticleEmitter(Entity* entity);
            
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
            
            /// Particletype
            ParticleType particleType;
            
            /// Size of the emitter (area of effect).
            glm::vec3 size;
            
            /// Minimum time for emitting particle.
            float minEmitTime = 0.03f;
            
            /// Maximum time for emitting particle.
            float maxEmitTime = 0.05f;
            
            /// Time to next emit (random value between min and max),
            float timeToNext = 0.1f;
            
            /// Type of emitter.
            EmitterType emitterType = EmitterType::POINT;
    };
}
