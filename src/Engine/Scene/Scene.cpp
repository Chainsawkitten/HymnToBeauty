#include "Scene.hpp"

#include "../Entity/Entity.hpp"

Scene::Scene() {
    
}

Scene::~Scene() {
    Clear();
}

Entity* Scene::CreateEntity() {
    Entity* entity = new Entity(this);
    entities.push_back(entity);
    return entity;
}

void Scene::Clear() {
    
}
