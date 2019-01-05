#pragma once

#include <glm/glm.hpp>
#include <Video/Shader/Shader.hpp>
#include <Video/Shader/ShaderProgram.hpp>

namespace Particles {
struct ParticlePos {
    float x;
    float y;
    float z;
    float w;
};

struct ParticleVelocity {
    float vx;
    float vy;
    float vz;
    float life;
};

struct ParticleColor {
    float cx;
    float cy;
    float cz;
    float ca;
};
} // namespace Particles

struct ParticleModelMatrix {
    glm::mat4 m = glm::mat4(1);
};

namespace Video {
class Texture2D;

/// Represents a particle system that can be simulated and rendered.
class ParticleSystemRenderer {
  public:
    struct EmitterSettings {
        /// Number of particles.
        int nr_particles = 1024;

        /// Position.
        glm::vec3 worldPos;

        /// Size.
        glm::vec2 size;

        /// Life (in seconds).
        float life;

        /// Lifetime (in seconds).
        float lifetime = 10;

        /// Initial velocity.
        glm::vec3 velocity;

        /// Start, mid and end of life alpha of particle.
        glm::vec3 alpha;

        /// Color of the particle.
        glm::vec3 color = glm::vec3(0, 0, 0);

        /// Fire rate.
        float rate = 1.0f;

        /// Texture index (for the texture atlas, left to right, top to bottom indexing)
        int textureIndex = 0;

        /// Multiply velocity.
        float velocityMultiplier = 10.0f;

        /// Gives spread to the particles
        int spread = 1;

        /// Used to give random velocity to particles at all time.
        glm::vec3 randomVec = glm::vec3(1, 1, 1);

        /// Scale.
        float scale = 1.0f;

        /// Particle mass.
        float mass = 0.01f;

        /// Alpha control
        float alpha_control = 1.0f;

        /// Nr Of new particles per emitt, max is 32.
        int nr_new_particles = 31;
    };

    EmitterSettings emitterSettings;

    /// Constructor.
    ParticleSystemRenderer();

    /// Construct a particle system with a given number of particles.
    /**
     * @param count Number of particles.
     */
    ParticleSystemRenderer(int count);

    /// Destructor.
    ~ParticleSystemRenderer();

    /// Initialize the particle system.
    void Init();

    /// Creates storage buffers for particles.
    void CreateStorageBuffers();

    /// Particles are sent to the compute shader and we compute their the new positions/velocities.
    /**
     * @param dt Deltatime.
     * @param settings Emitter settings.
     */
    void Update(float dt, EmitterSettings settings);

    /// Render the particles.
    /**
     * @param textureAtlas The texture atlas for the particles.
     * @param textureAtlasRows how many rows in texture atlas.
     * @param viewMatrix The camera's view matrix.
     * @param projectionMatrix The camera's projection matrix.
     * @param settings Settings for the emitter.
     */
    void Draw(Texture2D* textureAtlas, unsigned int textureAtlasRows, const glm::mat4& viewMatrix, const glm::mat4& ProjectionMatrix, ParticleSystemRenderer::EmitterSettings settings);

  private:
    Video::ShaderProgram* computeShaderProgram;

    Video::ShaderProgram* shaderProgram;

    int WORK_GROUP_SIZE = 128;

    unsigned int nr_particles = 1024 * 32;
    glm::vec2 particleShootIndex = glm::vec2(0, 30);
    int nr_new_particles = 31;
    float rate = 1000.0f;
    float delta_time = 0.0f;
    Particles::ParticlePos* points;
    Particles::ParticleVelocity* vels;
    Particles::ParticleColor* col;
    ParticleModelMatrix* rots;
    glm::mat4 rotMat;

    float timer = 0.0f;

    GLuint posSSbo;
    GLuint velSSbo;
    GLuint colSSbo;
    GLuint mmSSbo;
    GLuint m_glDrawVAO;
};
} // namespace Video
