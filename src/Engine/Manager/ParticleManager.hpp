#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>
#include <random>

class Entity;
class World;
class Shader;
class ShaderProgram;
class Texture2D;
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
         * @param world World to update.
         * @param time Time since last frame (in seconds).
         */
        void Update(World& world, float time);
        
        /// Update particle buffer.
        /**
         * Needs to be called before rendering (but only once a frame).
         * @param world The world to render.
         */
        void UpdateBuffer(World& world);

        /// Render the particles in a world.
        /**
         * @param world %World containing particles to render.
         * @param camera Camera through which to render.
         */
        void Render(World& world, const Entity* camera);
        
    private:
        ParticleManager();
        ~ParticleManager();
        ParticleManager(ParticleManager const&) = delete;
        void operator=(ParticleManager const&) = delete;
        
        // Decide where the emitter should emit before rendering.
        void EmitParticle(World& world, Component::ParticleEmitter* emitter);
        
        // Emit a particle at the given position.
        void EmitParticle(World& world, const glm::vec3& position, Component::ParticleEmitter* emitter);
        
        unsigned int maxParticleCount = 10000;
        
        std::random_device randomDevice;
        std::mt19937 randomEngine;
        
        // Shaders.
        Shader* vertexShader;
        Shader* geometryShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;

        // The number of rows in the texture atlas.
        float textureAtlasRowNumber = 4.f;

        // Texture atlas containing the particle textures.
        Texture2D* textureAtlas;

        // Vertex buffer.
        GLuint vertexBuffer = 0;
        GLuint vertexArray = 0;
        unsigned int vertexCount = 0;
};
