#include "World.hpp"

#include "../Entity/Entity.hpp"
#include "../Component/SuperComponent.hpp"
#include "../Manager/Managers.hpp"
#include "../Util/FileSystem.hpp"
#include <fstream>

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

Entity* World::GetRoot() const {
    return root;
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
    root = nullptr;
    
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

void World::Save(const std::string& filename) const {
    Json::Value root;
    
    for (Entity* entity : entities) {
        root.append(entity->Save());
    }
    
    std::ofstream file(filename);
    file << root;
    file.close();
}

void World::Load(const std::string& filename) {
    Clear();
    
    // Load Json document from file.
    if (FileSystem::FileExists(filename.c_str())) {
        Json::Value root;
        std::ifstream file(filename);
        file >> root;
        file.close();
        
        // Load entities.
        for (unsigned int i=0; i < root.size(); ++i) {
            Entity* entity = CreateEntity("");
            entity->Load(root[i]);
        }
    } else {
        root = CreateEntity("Root");
    }
}

void World::AddComponent(Component::SuperComponent* component, const std::type_info* componentType) {
    components[componentType].push_back(component);
}
