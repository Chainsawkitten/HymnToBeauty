#include "ParticleManager.hpp"

#include "../Scene/Scene.hpp"
#include "../Entity/Entity.hpp"
#include "../Component/ParticleEmitter.hpp"
#include "../Component/Transform.hpp"

ParticleManager::ParticleManager() {
    randomEngine.seed(randomDevice());
}

ParticleManager::~ParticleManager() {
    
}

unsigned int ParticleManager::GetMaxParticleCount() const {
    return maxParticleCount;
}

void ParticleManager::Update(Scene& scene, double time) {
    // Remove old particles.
    for (unsigned int i = 0; i < scene.GetParticleCount(); ++i) {
        scene.GetParticles()[i].life += static_cast<float>(time);
        if (scene.GetParticles()[i].life >= scene.GetParticles()[i].lifetime) {
            scene.GetParticles()[i--] = scene.GetParticles()[scene.GetParticleCount() - 1];
            scene.SetParticleCount(scene.GetParticleCount() - 1);
        }
    }
    
    // Spawn new particles from emitters.
    std::uniform_real_distribution<> zeroToOne(0, 1);
    std::vector<Component::ParticleEmitter*> particleEmitters = scene.GetComponents<Component::ParticleEmitter>();
    for (Component::ParticleEmitter* emitter : particleEmitters) {
        if (emitter->IsKilled())
            continue;
        
        emitter->timeToNext -= time;
        while (emitter->timeToNext < 0.0) {
            emitter->timeToNext += emitter->minEmitTime + zeroToOne(randomEngine) * (emitter->maxEmitTime - emitter->minEmitTime);
            EmitParticle(scene, emitter);
        }
    }
}

void ParticleManager::EmitParticle(Scene& scene, Component::ParticleEmitter* emitter) {
    Component::Transform* transform = emitter->entity->GetComponent<Component::Transform>();
    if (transform != nullptr) {
        glm::vec3 position(transform->position);
        if (emitter->emitterType == Component::ParticleEmitter::CUBOID) {
            std::uniform_real_distribution<float> randomSpread(-0.5f, 0.5f);
            glm::vec3 random(randomSpread(randomEngine), randomSpread(randomEngine), randomSpread(randomEngine));
            position += random * emitter->size;
        }
        EmitParticle(scene, position, emitter);
    }
}

void ParticleManager::EmitParticle(Scene& scene, const glm::vec3& position, Component::ParticleEmitter* emitter) {
    if (scene.GetParticleCount() < maxParticleCount) {
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
        
        scene.GetParticles()[scene.GetParticleCount()] = particle;
        scene.SetParticleCount(scene.GetParticleCount() + 1);
    }
}
