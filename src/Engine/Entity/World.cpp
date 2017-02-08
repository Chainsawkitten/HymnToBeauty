#include "World.hpp"

#include "../Entity/Entity.hpp"
#include "../Component/SuperComponent.hpp"
#include "../Manager/Managers.hpp"

World::World() {
    particles = new ParticleManager::Particle[Managers().particleManager->GetMaxParticleCount()];
}

World::~World() {
    Clear();
    
    delete[] particles;
}

Entity* World::CreateEntity(const std::string& name) {
    Entity* entity = new Entity(this, name);
    entities.push_back(entity);
    return entity;
}

const std::vector<Entity*>& World::GetEntities() const {
    return entities;
}

void World::RegisterUpdate(Entity* entity) {
    updateEntities.push_back(entity);
}

const std::vector<Entity*>& World::GetUpdateEntities() const {
    return updateEntities;
}

void World::Clear() {
    for (Entity* entity : entities)
        delete entity;
    entities.clear();
    
    for (auto& it : components) {
        for (Component::SuperComponent* component : it.second)
            delete component;
    }
    components.clear();
    
    particleCount = 0;
    updateEntities.clear();
}

void World::ClearKilled() {
    // Clear killed components.
    std::size_t i;
    for (auto& componentIt : components) {
        i = 0;
        while (i < componentIt.second.size()) {
            if (componentIt.second[i]->IsKilled()) {
                delete componentIt.second[i];
                componentIt.second[i] = componentIt.second[componentIt.second.size() - 1];
                componentIt.second.pop_back();
            } else {
                ++i;
            }
        }
    }
    
    // Clear killed entities.
    i = 0;
    while (i < entities.size()) {
        if (entities[i]->IsKilled()) {
            delete entities[i];
            entities[i] = entities[entities.size() - 1];
            entities.pop_back();
        } else {
            ++i;
        }
    }
}

ParticleManager::Particle* World::GetParticles() const {
    return particles;
}

unsigned int World::GetParticleCount() const {
    return particleCount;
}

void World::SetParticleCount(unsigned int particleCount) {
    this->particleCount = particleCount;
}

void World::AddComponent(Component::SuperComponent* component, const std::type_info* componentType) {
    components[componentType].push_back(component);
}
