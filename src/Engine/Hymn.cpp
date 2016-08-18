#include "Hymn.hpp"

#include "Util/FileSystem.hpp"
#include "Manager/Managers.hpp"
#include "Manager/RenderManager.hpp"
#include "Manager/ResourceManager.hpp"
#include "DefaultDiffuse.png.hpp"
#include "DefaultNormal.png.hpp"
#include "DefaultSpecular.png.hpp"
#include "DefaultGlow.png.hpp"
#include "Entity/Entity.hpp"
#include "Geometry/OBJModel.hpp"
#include "Texture/Texture2D.hpp"
#include <json/json.h>
#include <fstream>

using namespace std;

ActiveHymn::ActiveHymn() {
    defaultDiffuse = Managers().resourceManager->CreateTexture2D(DEFAULTDIFFUSE_PNG, DEFAULTDIFFUSE_PNG_LENGTH, true);
    defaultNormal = Managers().resourceManager->CreateTexture2D(DEFAULTNORMAL_PNG, DEFAULTNORMAL_PNG_LENGTH);
    defaultSpecular = Managers().resourceManager->CreateTexture2D(DEFAULTSPECULAR_PNG, DEFAULTSPECULAR_PNG_LENGTH);
    defaultGlow = Managers().resourceManager->CreateTexture2D(DEFAULTGLOW_PNG, DEFAULTGLOW_PNG_LENGTH);
    
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
    
    // Save entities.
    Json::Value entitiesNode;
    for (Entity* entity : activeScene.GetEntities()) {
        entitiesNode.append(entity->Save());
    }
    root["entities"] = entitiesNode;
    
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
    
    // Load models.
    const Json::Value modelsNode = root["models"];
    for (unsigned int i=0; i < modelsNode.size(); ++i) {
        Geometry::OBJModel* model = new Geometry::OBJModel();
        model->Load(modelsNode[i]);
        models.push_back(model);
    }
    
    // Load entities.
    const Json::Value entitiesNode = root["entities"];
    for (unsigned int i=0; i < entitiesNode.size(); ++i) {
        Entity* entity = activeScene.CreateEntity("");
        entity->Load(entitiesNode[i]);
    }
}

void ActiveHymn::Render() {
    Managers().renderManager->Render(activeScene);
}

ActiveHymn& Hymn() {
    return ActiveHymn::GetInstance();
}
