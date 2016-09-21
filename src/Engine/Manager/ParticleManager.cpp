#include "ParticleManager.hpp"

ParticleManager::ParticleManager() {
    
}

ParticleManager::~ParticleManager() {
    
}

unsigned int ParticleManager::GetMaxParticleCount() const {
    return maxParticleCount;
}

void ParticleManager::Update(Scene& scene, double time) {
    /// @todo Update particles and emitters.
}
