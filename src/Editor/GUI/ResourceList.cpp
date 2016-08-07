#include "ResourceList.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Geometry/Rectangle.hpp>
#include <Engine/Geometry/OBJModel.hpp>
#include <Engine/Font/Font.hpp>
#include <Engine/Texture/Texture2D.hpp>
#include "ABeeZee.ttf.hpp"
#include "Add.png.hpp"

#include <Engine/Hymn.hpp>
#include <Engine/Scene/Scene.hpp>
#include <Engine/Util/Input.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Physics/Rectangle.hpp>

using namespace GUI;

ResourceList::ResourceList(Widget* parent) : Widget(parent) {
    rectangle = Managers().resourceManager->CreateRectangle();
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    addTexture = Managers().resourceManager->CreateTexture2D(ADD_PNG, ADD_PNG_LENGTH);
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
    rect = Physics::Rectangle(position + glm::vec2(font->GetWidth("Models") + 5.f, 6.f), glm::vec2(10.f, 10.f));
    addModelHover = rect.Collide(mousePosition);
    
    position.y += (Hymn().models.size() + 1) * font->GetHeight();
    rect = Physics::Rectangle(position + glm::vec2(font->GetWidth("Textures") + 5.f, 6.f), glm::vec2(10.f, 10.f));
    addTextureHover = rect.Collide(mousePosition);
    
    if (Input()->Triggered(InputHandler::CLICK)) {
        if (addEntityHover) {
            // Add entity button pressed.
            Hymn().activeScene.CreateEntity("Entity #" + std::to_string(Hymn().entityNumber++));
        } else if (addModelHover) {
            // Add model button pressed.
            Geometry::OBJModel* model = new Geometry::OBJModel();
            model->name = "Model #" + std::to_string(Hymn().modelNumber++);
            Hymn().models.push_back(model);
        } else if (addTextureHover) {
            // Add texture button pressed.
            Hymn().textures.push_back(new Texture2D());
        } else {
            position  = GetPosition();
            
            // Check if entity selected.
            for (Entity* entity : Hymn().activeScene.GetEntities()) {
                position.y += font->GetHeight();
                rect = Physics::Rectangle(position, glm::vec2(size.x, font->GetHeight()));
                if (rect.Collide(mousePosition)) {
                    selectedEntity = entity;
                    selectedModel = nullptr;
                    if (hasEntitySelectedCallback)
                        entitySelectedCallback(entity);
                    break;
                }
            }
            
            position  = GetPosition();
            position.y += (1 + Hymn().activeScene.GetEntities().size()) * font->GetHeight();
            
            // Check if model selected.
            for (Geometry::OBJModel* model : Hymn().models) {
                position.y += font->GetHeight();
                rect = Physics::Rectangle(position, glm::vec2(size.x, font->GetHeight()));
                if (rect.Collide(mousePosition)) {
                    selectedEntity = nullptr;
                    selectedModel = model;
                    if (hasModelSelectedCallback)
                        modelSelectedCallback(model);
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
    
    for (Entity* entity : Hymn().activeScene.GetEntities()) {
        // Render background if selected.
        if (selectedEntity == entity) {
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
            rectangle->Render(position, glm::vec2(size.x, font->GetHeight()), color, screenSize);
        }
        
        font->RenderText(entity->name.c_str(), position + glm::vec2(20.f, 0.f), GetSize().x, screenSize);
        position.y += font->GetHeight();
    }
    
    font->RenderText("Models", position, GetSize().x, screenSize);
    addTexture->Render(position + glm::vec2(font->GetWidth("Models") + 5.f, 6.f), glm::vec2(addTexture->GetWidth(), addTexture->GetHeight()), screenSize, addModelHover ? 1.f : 0.5f);
    position.y += font->GetHeight();
    
    for (Geometry::OBJModel* model : Hymn().models) {
        // Render background if selected.
        if (selectedModel == model) {
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
            rectangle->Render(position, glm::vec2(size.x, font->GetHeight()), color, screenSize);
        }
        
        font->RenderText(model->name.c_str(), position + glm::vec2(20.f, 0.f), GetSize().x, screenSize);
        position.y += font->GetHeight();
    }
    
    font->RenderText("Textures", position, GetSize().x, screenSize);
    addTexture->Render(position + glm::vec2(font->GetWidth("Textures") + 5.f, 6.f), glm::vec2(addTexture->GetWidth(), addTexture->GetHeight()), screenSize, addTextureHover ? 1.f : 0.5f);
    position.y += font->GetHeight();
    
    unsigned int id = 0U;
    for (Texture2D* texture : Hymn().textures) {
        // Render background if selected.
        /*if (selectedTexture == texture) {
            color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
            rectangle->Render(position, glm::vec2(size.x, font->GetHeight()), color, screenSize);
        }*/
        
        font->RenderText(("Texture #" + std::to_string(id)).c_str(), position + glm::vec2(20.f, 0.f), GetSize().x, screenSize);
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

void ResourceList::SetModelSelectedCallback(std::function<void(Geometry::OBJModel*)> callback) {
    hasModelSelectedCallback = true;
    modelSelectedCallback = callback;
}
