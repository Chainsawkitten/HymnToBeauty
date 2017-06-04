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
                
                /// The average lifetime of the particle (in seconds).
                float averageLifetime = 1.f;
                
                /// The lifetime variance of the particle (in seconds).
                float lifetimeVariance = 0.f;
                
                /// The average size of the particle.
                glm::vec2 averageSize = glm::vec2(1.f, 1.f);
                
                /// The size variance of the particle.
                glm::vec2 sizeVariance = glm::vec2(0.f, 0.f);
                
                /// Whether to scale all axes individually or uniformly.
                bool uniformScaling = true;
                
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
            
            /// Average time for emitting particle (in seconds).
            float averageEmitTime = 0.04f;
            
            /// Time variance for emitting particle (in seconds).
            float emitTimeVariance = 0.01f;
            
            /// Time to next emit (random value between min and max),
            float timeToNext = 0.1f;
            
            /// Type of emitter.
            EmitterType emitterType = EmitterType::POINT;
            
            /// Whether to preview the emitter in the editor.
            bool preview = false;
    };
}
