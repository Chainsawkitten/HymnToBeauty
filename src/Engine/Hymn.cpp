#include "Hymn.hpp"

#include "Util/FileSystem.hpp"
#include "Manager/Managers.hpp"
#include "Manager/RenderManager.hpp"
#include "Entity/Entity.hpp"
#include "Component/Transform.hpp"
#include "Component/Lens.hpp"
#include "Geometry/OBJModel.hpp"
#include "Texture/Texture2D.hpp"
#include <json/json.h>
#include <fstream>

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
    
    entityNumber = 1U;
    
    for (Geometry::OBJModel* model : models) {
        delete model;
    }
    models.clear();
    modelNumber = 0U;
    
    for (Texture2D* texture : textures) {
        delete texture;
    }
    textures.clear();
    textureNumber = 0U;
    
    Entity* camera = activeScene.CreateEntity("Camera");
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
    FileSystem::CreateDirectory((path + FileSystem::DELIMITER + "Textures").c_str());
}

void ActiveHymn::Save() const {
    Json::Value root;
    
    // Save textures.
    Json::Value texturesNode;
    for (Texture2D* texture : textures) {
        texturesNode.append(texture->Save());
    }
    root["textures"] = texturesNode;
    
    // Save models.
    Json::Value modelsNode;
    for (Geometry::OBJModel* model : models) {
        modelsNode.append(model->Save());
    }
    root["models"] = modelsNode;
    
    /// @todo Save entities.
    /// @todo Save components.
    
    // Save to file.
    ofstream file(path + FileSystem::DELIMITER + "Hymn.json");
    file << root;
    file.close();
}

void ActiveHymn::Load(const string& path) {
    Clear();
    this->path = path;
    
    // Load Json document from file.
    Json::Value root;
    ifstream file(path + FileSystem::DELIMITER + "Hymn.json");
    file >> root;
    file.close();
    
    // Load textures.
    const Json::Value texturesNode = root["textures"];
    for (unsigned int i=0; i < texturesNode.size(); ++i) {
        Texture2D* texture = new Texture2D();
        texture->Load(texturesNode[i]);
        textures.push_back(texture);
    }
    
    /// @todo Load models.
    /// @todo Load entities.
    /// @todo Load components.
}

void ActiveHymn::Render(const glm::vec2& screenSize) {
    Managers().renderManager->Render(activeScene, screenSize);
}

ActiveHymn& Hymn() {
    return ActiveHymn::GetInstance();
}
