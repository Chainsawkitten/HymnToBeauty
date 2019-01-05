#include "Video/ParticleSystemRenderer.hpp"

#include "ComputeParticleShader.comp.hpp"
#include "DefaultParticleShader.vert.hpp"
#include "DefaultParticleShader.geom.hpp"
#include "DefaultParticleShader.frag.hpp"
#include "Texture/Texture2D.hpp"
#include <time.h>
#include <Utility/Log.hpp>

using namespace Video;

ParticleSystemRenderer::ParticleSystemRenderer() {
    points = new Particles::ParticlePos[nr_particles];
    vels = new Particles::ParticleVelocity[nr_particles];
    col = new Particles::ParticleColor[nr_particles];
    rots = new ParticleModelMatrix[nr_particles];
    srand(time(NULL));

    // Temporary rotation matrix for future rotation of particles.
    rotMat[0][0] = cosf(1);
    rotMat[0][1] = -sinf(1);
    rotMat[0][2] = 0;
    rotMat[0][3] = 1;
    rotMat[1][0] = sinf(1);
    rotMat[1][1] = cosf(1);
    rotMat[1][2] = 0;
    rotMat[1][3] = 1;
    rotMat[2][0] = 0;
    rotMat[2][1] = 0;
    rotMat[2][2] = 0;
    rotMat[2][3] = 1;
    rotMat[3][0] = 0;
    rotMat[3][1] = 0;
    rotMat[3][2] = 0;
    rotMat[3][3] = 1;
}

ParticleSystemRenderer::ParticleSystemRenderer(int count) {
    points = new Particles::ParticlePos[count];
    vels = new Particles::ParticleVelocity[count];
    col = new Particles::ParticleColor[count];
    rots = new ParticleModelMatrix[count];
    srand(time(NULL));

    // Temporary rotation matrix for future rotation of particles.
    rotMat[0][0] = cosf(10);
    rotMat[0][1] = -sinf(10);
    rotMat[0][2] = 0;
    rotMat[0][3] = 1;
    rotMat[1][0] = sinf(10);
    rotMat[1][1] = cosf(10);
    rotMat[1][2] = 0;
    rotMat[1][3] = 1;
    rotMat[2][0] = 0;
    rotMat[2][1] = 0;
    rotMat[2][2] = 0;
    rotMat[2][3] = 1;
    rotMat[3][0] = 0;
    rotMat[3][1] = 0;
    rotMat[3][2] = 0;
    rotMat[3][3] = 1;
}

ParticleSystemRenderer::~ParticleSystemRenderer() {
    delete shaderProgram;
    delete computeShaderProgram;

    glDeleteBuffers(1, &posSSbo);
    glDeleteBuffers(1, &velSSbo);
    glDeleteBuffers(1, &colSSbo);
    glDeleteVertexArrays(1, &m_glDrawVAO);
}

void ParticleSystemRenderer::Init() {
    // Load shaders.
    Video::Shader* vertexShader = new Video::Shader(DEFAULTPARTICLESHADER_VERT, DEFAULTPARTICLESHADER_VERT_LENGTH, GL_VERTEX_SHADER);
    Video::Shader* geometryShader = new Video::Shader(DEFAULTPARTICLESHADER_GEOM, DEFAULTPARTICLESHADER_GEOM_LENGTH, GL_GEOMETRY_SHADER);
    Video::Shader* fragmentShader = new Video::Shader(DEFAULTPARTICLESHADER_FRAG, DEFAULTPARTICLESHADER_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = new Video::ShaderProgram({vertexShader, geometryShader, fragmentShader});
    delete vertexShader;
    delete geometryShader;
    delete fragmentShader;

    Video::Shader* computeShader = new Video::Shader(COMPUTEPARTICLESHADER_COMP, COMPUTEPARTICLESHADER_COMP_LENGTH, GL_COMPUTE_SHADER);
    computeShaderProgram = new Video::ShaderProgram({computeShader});
    delete computeShader;
}

void ParticleSystemRenderer::CreateStorageBuffers() {
    // Positions.
    for (unsigned int i = 0; i < this->nr_particles; ++i) {
        points[i].x = 0.0f;
        points[i].y = 0.0f;
        points[i].z = 0.0f;
        points[i].w = 0.0f;
    }

    glGenBuffers(1, &posSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, posSSbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particles::ParticlePos) * nr_particles, &points[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, posSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    // Binding vertex array to get position in vertexshader.
    glGenVertexArrays(1, &m_glDrawVAO);
    glBindVertexArray(m_glDrawVAO);
    glBindBuffer(GL_ARRAY_BUFFER, posSSbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particles::ParticlePos) * nr_particles, &points[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Particles::ParticlePos), 0);

    glBindVertexArray(0);

    // Velocity.
    for (unsigned int i = 0; i < this->nr_particles; ++i) {
        vels[i].vx = 0.0f;
        vels[i].vy = 0.0f;
        vels[i].vz = 0.0f;
        vels[i].life = 0.0f;
    }

    glGenBuffers(1, &velSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velSSbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particles::ParticlePos) * nr_particles, &vels[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    // Color.
    for (unsigned int i = 0; i < this->nr_particles; ++i) {
        col[i].cx = 0.0f;
        col[i].cy = 0.0f;
        col[i].cz = 0.0f;
        col[i].ca = 0.0f;
    }

    glGenBuffers(1, &colSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, colSSbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particles::ParticlePos) * nr_particles, &col[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, colSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

    // Binding vertex array to get color in fragmentshader.
    glBindVertexArray(m_glDrawVAO);
    glBindBuffer(GL_ARRAY_BUFFER, colSSbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Particles::ParticlePos) * nr_particles, &col[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particles::ParticleColor), 0);

    glBindVertexArray(0);

    delete[] points;
    delete[] vels;
    delete[] col;
    delete[] rots;
}

void ParticleSystemRenderer::Update(float dt, ParticleSystemRenderer::EmitterSettings settings) {
    timer += dt;

    nr_particles = settings.nr_particles;

    computeShaderProgram->Use();

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, posSSbo, 0, nr_particles * sizeof(Particles::ParticlePos));
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, velSSbo, 0, nr_particles * sizeof(Particles::ParticleVelocity));
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 2, colSSbo, 0, nr_particles * sizeof(Particles::ParticleColor));

    glUniform3fv(computeShaderProgram->GetUniformLocation("Velocity"), 1, &glm::vec3(settings.velocity.x, settings.velocity.y, settings.velocity.z)[0]);
    glUniform1f(computeShaderProgram->GetUniformLocation("rate"), settings.rate);
    glUniform2fv(computeShaderProgram->GetUniformLocation("ShootIndex"), 1, &particleShootIndex[0]);
    glUniform1f(computeShaderProgram->GetUniformLocation("timer"), timer);
    glUniform1f(computeShaderProgram->GetUniformLocation("lifetime"), settings.lifetime);
    glUniform1f(computeShaderProgram->GetUniformLocation("speed"), settings.velocityMultiplier);
    glUniform1f(computeShaderProgram->GetUniformLocation("mass"), settings.mass);
    glUniform3fv(computeShaderProgram->GetUniformLocation("modColor"), 1, &settings.color[0]);
    glUniform3fv(computeShaderProgram->GetUniformLocation("worldPosition"), 1, &glm::vec3(settings.worldPos)[0]);
    glUniform1i(computeShaderProgram->GetUniformLocation("nr_particles"), nr_particles);

    nr_new_particles = settings.nr_new_particles;
    particleShootIndex.y = settings.nr_new_particles - 1.f;

    glm::vec3 randomVec[32];

    for (unsigned int i = 0; i < 32; i++) {
        randomVec[i] = glm::normalize(glm::vec3(settings.randomVec.x + (rand() % settings.spread) - (settings.spread / 2), settings.randomVec.y + ((rand() % settings.spread) - (settings.spread / 2)), settings.randomVec.z + (rand() % settings.spread) - (settings.spread / 2)));
    }

    glUniform3fv(computeShaderProgram->GetUniformLocation("randomVec"), 32, &randomVec[0].x);
    glUniform1f(computeShaderProgram->GetUniformLocation("alphaControl"), settings.alpha_control);

    glDispatchCompute(static_cast<int>(std::ceil((float)nr_particles / 128)), 1, 1);
    glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glUseProgram(0);

    if (timer >= settings.rate) {
        particleShootIndex.x += nr_new_particles;
        particleShootIndex.y = particleShootIndex.x + nr_new_particles;
        if (particleShootIndex.y > nr_particles) {
            particleShootIndex.y = static_cast<float>(nr_new_particles);
            particleShootIndex.x = 0;
        }
        timer = 0.0f;
    }
}

void ParticleSystemRenderer::Draw(Texture2D* textureAtlas, unsigned int textureAtlasRows, const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, ParticleSystemRenderer::EmitterSettings settings) {
    // Blending.
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnablei(GL_BLEND, 0);
    glBlendFunci(0, GL_SRC_ALPHA, GL_ONE);

    shaderProgram->Use();
    glBindVertexArray(m_glDrawVAO);

    glUniformMatrix4fv(shaderProgram->GetUniformLocation("viewMatrix"), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(shaderProgram->GetUniformLocation("projMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniform1i(shaderProgram->GetUniformLocation("baseImage"), 0);
    glUniform1i(shaderProgram->GetUniformLocation("textureIndex"), settings.textureIndex);
    glUniform1f(shaderProgram->GetUniformLocation("scale"), settings.scale);

    // Base image texture.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureAtlas->GetTextureID());

    // Send the texture to shader.
    glUniform1f(shaderProgram->GetUniformLocation("textureAtlasRows"), static_cast<GLfloat>(textureAtlasRows));

    glDrawArrays(GL_POINTS, 0, nr_particles);

    glDisablei(GL_BLEND, 0);
    glDisablei(GL_BLEND, 1);

    // Cleanup.
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
}
