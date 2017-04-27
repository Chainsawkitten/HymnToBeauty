#include "ParticleManager.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include "../Entity/World.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Component/Lens.hpp"
#include "../MainWindow.hpp"
#include "../Manager/Managers.hpp"
#include "../Manager/ResourceManager.hpp"
#include "../Shader/Shader.hpp"
#include "../Shader/ShaderProgram.hpp"
#include "../Texture/Texture2D.hpp"
#include "Particle.vert.hpp"
#include "Particle.geom.hpp"
#include "Particle.frag.hpp"
#include "ParticleAtlas.png.hpp"

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

ParticleManager::ParticleManager() {
    randomEngine.seed(randomDevice());
    
    // Load shaders.
    vertexShader = Managers().resourceManager->CreateShader(PARTICLE_VERT, PARTICLE_VERT_LENGTH, GL_VERTEX_SHADER);
    geometryShader = Managers().resourceManager->CreateShader(PARTICLE_GEOM, PARTICLE_GEOM_LENGTH, GL_GEOMETRY_SHADER);
    fragmentShader = Managers().resourceManager->CreateShader(PARTICLE_FRAG, PARTICLE_FRAG_LENGTH, GL_FRAGMENT_SHADER);
    shaderProgram = Managers().resourceManager->CreateShaderProgram({ vertexShader, geometryShader, fragmentShader });
    
    textureAtlas = Managers().resourceManager->CreateTexture2D(PARTICLEATLAS_PNG, PARTICLEATLAS_PNG_LENGTH);
    
    // Vertex buffer
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, maxParticleCount * sizeof(Particle), NULL, GL_DYNAMIC_DRAW);
    
    // Define vertex data layout.
    glGenVertexArrays(1, &vertexArray);
    glBindVertexArray(vertexArray);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glEnableVertexAttribArray(7);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), BUFFER_OFFSET(sizeof(float) * 3));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), BUFFER_OFFSET(sizeof(float) * 5));
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), BUFFER_OFFSET(sizeof(float) * 6));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), BUFFER_OFFSET(sizeof(float) * 7));
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), BUFFER_OFFSET(sizeof(float) * 10));
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), BUFFER_OFFSET(sizeof(float) * 13));
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), BUFFER_OFFSET(sizeof(float) * 16));
    
    glBindVertexArray(0);
}

ParticleManager::~ParticleManager() {
    Managers().resourceManager->FreeShaderProgram(shaderProgram);
    Managers().resourceManager->FreeShader(vertexShader);
    Managers().resourceManager->FreeShader(geometryShader);
    Managers().resourceManager->FreeShader(fragmentShader);
    Managers().resourceManager->FreeTexture2D(textureAtlas);
    
    glDeleteBuffers(1, &vertexBuffer);
}

unsigned int ParticleManager::GetMaxParticleCount() const {
    return maxParticleCount;
}

void ParticleManager::Update(World& world, float time) {
    // Remove old particles.
    for (unsigned int i = 0; i < world.GetParticleCount(); ++i) {
        world.GetParticles()[i].life += time;
        if (world.GetParticles()[i].life >= world.GetParticles()[i].lifetime) {
            world.GetParticles()[i--] = world.GetParticles()[world.GetParticleCount() - 1];
            world.SetParticleCount(world.GetParticleCount() - 1);
        }
    }
    
    // Spawn new particles from emitters.
    std::uniform_real_distribution<float> zeroToOne(0.f, 1.f);
    std::vector<Component::ParticleEmitter*> particleEmitters = world.GetComponents<Component::ParticleEmitter>();
    for (Component::ParticleEmitter* emitter : particleEmitters) {
        if (emitter->IsKilled())
            continue;
        
        emitter->timeToNext -= time;
        while (emitter->timeToNext < 0.f) {
            emitter->timeToNext += emitter->minEmitTime + zeroToOne(randomEngine) * (emitter->maxEmitTime - emitter->minEmitTime);
            EmitParticle(world, emitter);
        }
    }
}

void ParticleManager::UpdateBuffer(World& world) {
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, world.GetParticleCount() * sizeof(Particle), world.GetParticles());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleManager::Render(World& world, Entity* camera) {
    if (world.GetParticleCount() > 0) {
        // Don't write to depth buffer.
        GLboolean depthWriting;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWriting);
        glDepthMask(GL_FALSE);
        
        // Blending
        glEnablei(GL_BLEND, 0);
        glEnablei(GL_BLEND, 1);
        glBlendFunci(0, GL_SRC_ALPHA, GL_ONE);
        glBlendFunci(1, GL_SRC_ALPHA, GL_ONE);
        
        shaderProgram->Use();
        
        glBindVertexArray(vertexArray);
        
        glUniform1i(shaderProgram->GetUniformLocation("baseImage"), 0);
        
        // Base image texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureAtlas->GetTextureID());
        
        // Send the matrices to the shader.
        glm::mat4 viewMat = camera->GetCameraOrientation() * glm::translate(glm::mat4(), -camera->position);
        glm::mat4 projectionMat = camera->GetComponent<Component::Lens>()->GetProjection(MainWindow::GetInstance()->GetSize());
        glm::mat4 viewProjectionMat = projectionMat * viewMat;
        glm::vec3 up(glm::inverse(camera->GetCameraOrientation())* glm::vec4(0, 1, 0, 1));
        
        glUniform3fv(shaderProgram->GetUniformLocation("cameraPosition"), 1, &camera->position[0]);
        glUniform3fv(shaderProgram->GetUniformLocation("cameraUp"), 1, &up[0]);
        glUniformMatrix4fv(shaderProgram->GetUniformLocation("viewProjectionMatrix"), 1, GL_FALSE, &viewProjectionMat[0][0]);
        glUniform1fv(shaderProgram->GetUniformLocation("textureAtlasRows"), 1, &textureAtlasRowNumber);
        
        // Draw the triangles
        glDrawArrays(GL_POINTS, 0, world.GetParticleCount());
        
        // Reset state values we've changed.
        glDepthMask(depthWriting);
        glDisablei(GL_BLEND, 0);
        glDisablei(GL_BLEND, 1);
        
        glUseProgram(0);
        glBindVertexArray(0);
    }
}

const Texture2D* ParticleManager::GetTextureAtlas() const {
    return textureAtlas;
}

void ParticleManager::EmitParticle(World& world, Component::ParticleEmitter* emitter) {
    glm::vec3 position(emitter->entity->GetWorldPosition());
    if (emitter->emitterType == Component::ParticleEmitter::CUBOID) {
        std::uniform_real_distribution<float> randomSpread(-0.5f, 0.5f);
        glm::vec3 random(randomSpread(randomEngine), randomSpread(randomEngine), randomSpread(randomEngine));
        position += random * emitter->size;
    }
    EmitParticle(world, position, emitter);
}

void ParticleManager::EmitParticle(World& world, const glm::vec3& position, Component::ParticleEmitter* emitter) {
    if (world.GetParticleCount() < maxParticleCount) {
        Particle particle;
        std::uniform_real_distribution<float> zeroToOne(0.f, 1.f);
        particle.worldPos = position;
        particle.life = 0.f;
        particle.lifetime = emitter->particleType.minLifetime + zeroToOne(randomEngine) * (emitter->particleType.maxLifetime - emitter->particleType.minLifetime);
        particle.textureIndex = static_cast<float>(emitter->particleType.textureIndex);
        particle.alpha[0] = emitter->particleType.startAlpha;
        particle.alpha[1] = emitter->particleType.midAlpha;
        particle.alpha[2] = emitter->particleType.endAlpha;
        particle.color = emitter->particleType.color;
        
        if (emitter->particleType.uniformScaling) {
            particle.size = emitter->particleType.minSize + zeroToOne(randomEngine) * (emitter->particleType.maxSize - emitter->particleType.minSize);
        } else {
            particle.size.x = emitter->particleType.minSize.x + zeroToOne(randomEngine) * (emitter->particleType.maxSize.x - emitter->particleType.minSize.x);
            particle.size.y = emitter->particleType.minSize.y + zeroToOne(randomEngine) * (emitter->particleType.maxSize.y - emitter->particleType.minSize.y);
        }
        
        particle.velocity.x = emitter->particleType.minVelocity.x + zeroToOne(randomEngine) * (emitter->particleType.maxVelocity.x - emitter->particleType.minVelocity.x);
        particle.velocity.y = emitter->particleType.minVelocity.y + zeroToOne(randomEngine) * (emitter->particleType.maxVelocity.y - emitter->particleType.minVelocity.y);
        particle.velocity.z = emitter->particleType.minVelocity.z + zeroToOne(randomEngine) * (emitter->particleType.maxVelocity.z - emitter->particleType.minVelocity.z);
        
        world.GetParticles()[world.GetParticleCount()] = particle;
        world.SetParticleCount(world.GetParticleCount() + 1);
    }
}
