#pragma once

#include <glm/glm.hpp>
#include <random>

class Scene;
namespace Component {
    class ParticleEmitter;
}

/// Handles particles.
class ParticleManager {
    friend class Hub;
    
    public:
        /// A particle in the particle system.
        struct Particle {
                /// Position.
                glm::vec3 worldPos;
                
                /// Size.
                glm::vec2 size;
                
                /// Life (in seconds).
                float life;
                
                /// Lifetime (in seconds).
                float lifetime;
                
                /// Initial velocity.
                glm::vec3 velocity;
                
                /// Start, mid and end of life alpha of particle.
                glm::vec3 alpha;
                
                /// Color of the particle.
                glm::vec3 color;
                
                /// Texture index (for the texture atlas, left to right, top to bottom indexing)
                float textureIndex;
        };
        
        /// Get the maximum amount of particles.
        /**
         * @return Maximum amount of particles.
         */
        unsigned int GetMaxParticleCount() const;
        
        /// Update all the system's particles, spawn new particles etc.
        /**
         * @param scene Scene to update.
         * @param time Time since last frame (in seconds).
         */
        void Update(Scene& scene, double time);
        
    private:
        ParticleManager();
        ~ParticleManager();
        ParticleManager(Particle const&) = delete;
        void operator=(Particle const&) = delete;
        
        // Decide where the emitter should emit before rendering.
        void EmitParticle(Scene& scene, Component::ParticleEmitter* emitter);
        
        // Emit a particle at the given position.
        void EmitParticle(Scene& scene, const glm::vec3& position, Component::ParticleEmitter* emitter);
        
        unsigned int maxParticleCount = 10000;
        
        std::random_device randomDevice;
        std::mt19937 randomEngine;
};
