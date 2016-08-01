#include "ResourceList.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Geometry/Cube.hpp>
#include <Engine/Geometry/OBJModel.hpp>
#include <Engine/Font/Font.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include "ABeeZee.ttf.hpp"
#include "Add.png.hpp"

#include <Engine/Hymn.hpp>
#include <Engine/Scene/Scene.hpp>
#include <Engine/Util/Input.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Transform.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Physics/Rectangle.hpp>

using namespace GUI;

ResourceList::ResourceList(Widget* parent) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    addTexture = Managers().resourceManager->CreateTexture2D(ADD_PNG, ADD_PNG_LENGTH);
    
    addEntityHover = false;
    selectedEntity = nullptr;
    hasEntitySelectedCallback = false;
    
    addMeshHover = false;
    selectedMesh = nullptr;
}

ResourceList::~ResourceList() {
    Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    Managers().resourceManager->FreeTexture2D(addTexture);
}

void ResourceList::Update() {
    glm::vec2 mousePosition(Input()->CursorX(), Input()->CursorY());
    glm::vec2 position(GetPosition());
    Physics::Rectangle rect(position + glm::vec2(font->GetWidth("Entities") + 5.f, 6.f), glm::vec2(10.f, 10.f));
    addEntityHover = rect.Collide(mousePosition);
    
    position.y += (Hymn().activeScene.GetEntities().size() + 1) * font->GetHeight();
    rect = Physics::Rectangle(position + glm::vec2(font->GetWidth("Meshes") + 5.f, 6.f), glm::vec2(10.f, 10.f));
    addMeshHover = rect.Collide(mousePosition);
    
    if (Input()->Triggered(InputHandler::CLICK)) {
        if (addEntityHover) {
            // Add entity button pressed.
            Entity* cube = Hymn().activeScene.CreateEntity();
            cube->AddComponent<Component::Transform>();
            Component::Mesh* cubeMesh = cube->AddComponent<Component::Mesh>();
            cubeMesh->geometry = Managers().resourceManager->CreateCube();
        } else if (addMeshHover) {
            // Add mesh button pressed.
            Hymn().meshes.push_back(new Geometry::OBJModel());
        } else {
            position  = GetPosition();
            
            // Check if entity selected.
            for (Entity* entity : Hymn().activeScene.GetEntities()) {
                position.y += font->GetHeight();
                rect = Physics::Rectangle(position, glm::vec2(size.x, font->GetHeight()));
                if (rect.Collide(mousePosition)) {
                    selectedEntity = entity;
                    if (hasEntitySelectedCallback)
                        entitySelectedCallback(entity);
                    break;
                }
            }
            
            position  = GetPosition();
            position.y += (1 + Hymn().activeScene.GetEntities().size()) * font->GetHeight();
            
            // Check if mesh selected.
            for (Geometry::OBJModel* mesh : Hymn().meshes) {
                position.y += font->GetHeight();
                rect = Physics::Rectangle(position, glm::vec2(size.x, font->GetHeight()));
                if (rect.Collide(mousePosition)) {
                    selectedMesh = mesh;
                    /*if (hasEntitySelectedCallback)
                        entitySelectedCallback(mesh);*/
                    break;
                }
            }
        }
    }
}

void ResourceList::Render(const glm::vec2& screenSize) {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    glm::vec2 position = GetPosition();
    rectangle->Render(position, size, color, screenSize);
    
    font->SetColor(glm::vec3(1.f, 1.f, 1.f));
    font->RenderText("Entities", position, GetSize().x, screenSize);
    addTexture->Render(position + glm::vec2(font->GetWidth("Entities") + 5.f, 6.f), glm::vec2(addTexture->GetWidth(), addTexture->GetHeight()), screenSize, addEntityHover ? 1.f : 0.5f);
    position.y += font->GetHeight();
    
    unsigned int id = 0U;
    for (Entity* entity : Hymn().activeScene.GetEntities()) {
        // Render background if selected.
        if (selectedEntity == entity) {
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
            rectangle->Render(position, glm::vec2(size.x, font->GetHeight()), color, screenSize);
        }
        
        font->RenderText(("Entity #" + std::to_string(id)).c_str(), position + glm::vec2(20.f, 0.f), GetSize().x, screenSize);
        position.y += font->GetHeight();
        ++id;
    }
    
    font->RenderText("Meshes", position, GetSize().x, screenSize);
    addTexture->Render(position + glm::vec2(font->GetWidth("Meshes") + 5.f, 6.f), glm::vec2(addTexture->GetWidth(), addTexture->GetHeight()), screenSize, addMeshHover ? 1.f : 0.5f);
    position.y += font->GetHeight();
    
    id = 0U;
    for (Geometry::OBJModel* mesh : Hymn().meshes) {
        // Render background if selected.
        if (selectedMesh == mesh) {
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
            rectangle->Render(position, glm::vec2(size.x, font->GetHeight()), color, screenSize);
        }
        
        font->RenderText(("Mesh #" + std::to_string(id)).c_str(), position + glm::vec2(20.f, 0.f), GetSize().x, screenSize);
        position.y += font->GetHeight();
        ++id;
    }
}

glm::vec2 ResourceList::GetSize() const {
    return size;
}

void ResourceList::SetSize(const glm::vec2& size) {
    this->size = size;
}

void ResourceList::SetEntitySelectedCallback(std::function<void(Entity*)> callback) {
    hasEntitySelectedCallback = true;
    entitySelectedCallback = callback;
}
