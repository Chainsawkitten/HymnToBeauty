#include "Hymn.hpp"

#include "Util/FileSystem.hpp"
#include "Manager/Managers.hpp"
#include "Manager/RenderManager.hpp"
#include "Entity/Entity.hpp"
#include "Component/Transform.hpp"
#include "Component/Lens.hpp"
#include "Geometry/OBJModel.hpp"

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
    
    for (Geometry::OBJModel* model : models) {
        delete model;
    }
    models.clear();
    modelNumber = 0U;
    
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
    FileSystem::CreateDirectory((path + FileSystem::DELIMITER + "Models").c_str());
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
