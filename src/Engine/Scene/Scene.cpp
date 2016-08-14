#include "Scene.hpp"

#include "../Entity/Entity.hpp"
#include "../Component/SuperComponent.hpp"

Scene::Scene() {
    
}

Scene::~Scene() {
    Clear();
}

Entity* Scene::CreateEntity(const std::string& name) {
    Entity* entity = new Entity(this, name);
    entities.push_back(entity);
    return entity;
}

const std::vector<Entity*>& Scene::GetEntities() const {
    return entities;
}

void Scene::Clear() {
    for (Entity* entity : entities)
        delete entity;
    entities.clear();
    
    for (auto& it : components) {
        for (Component::SuperComponent* component : it.second)
            delete component;
    }
    components.clear();
}

void Scene::ClearKilled() {
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
}

void Scene::AddComponent(Component::SuperComponent* component, const std::type_info* componentType) {
    components[componentType].push_back(component);
}
