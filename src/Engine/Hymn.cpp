#include "Hymn.hpp"

#include "Util/FileSystem.hpp"
#include "Manager/Managers.hpp"
#include "Manager/RenderManager.hpp"
#include "Manager/ResourceManager.hpp"
#include "Entity/Entity.hpp"
#include "Component/Transform.hpp"
#include "Component/Mesh.hpp"
#include "Component/Lens.hpp"
#include "Geometry/Cube.hpp"

using namespace std;

ActiveHymn::ActiveHymn() {
    Clear();
}

ActiveHymn& ActiveHymn::GetInstance() {
    static ActiveHymn ActiveHymn;
    
    return ActiveHymn;
}

void ActiveHymn::Clear() {
    path = "";
    activeScene.Clear();
    
    Entity* camera = activeScene.CreateEntity();
    Component::Transform* cameraTransform = camera->AddComponent<Component::Transform>();
    cameraTransform->position = glm::vec3(0.f, 0.f, 10.f);
    camera->AddComponent<Component::Lens>();
}

const string& ActiveHymn::GetPath() const {
    return path;
}

void ActiveHymn::SetPath(const string& path) {
    this->path = path;
    FileSystem::CreateDirectory(path.c_str());
}

void ActiveHymn::Load(const string& path) {
    Clear();
    this->path = path;
}

void ActiveHymn::Render(const glm::vec2& screenSize) {
    Managers().renderManager->Render(activeScene, screenSize);
}

ActiveHymn& Hymn() {
    return ActiveHymn::GetInstance();
}
