#include "Mesh.hpp"

using namespace Component;

Mesh::Mesh(Entity* entity) : SuperComponent(entity) {
    geometry = nullptr;
}

Mesh::~Mesh() {
    
}
