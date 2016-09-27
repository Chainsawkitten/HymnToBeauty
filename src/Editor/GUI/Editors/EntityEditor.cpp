#include "EntityEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"
#include <Engine/Texture/Texture2D.hpp>
#include "Add.png.hpp"
#include "Subtract.png.hpp"
#include <Engine/Geometry/Rectangle.hpp>
#include "ComponentEditor/ComponentAdder.hpp"
#include "ComponentEditor/TransformEditor.hpp"
#include "ComponentEditor/PhysicsEditor.hpp"
#include "ComponentEditor/LensEditor.hpp"
#include "ComponentEditor/MeshEditor.hpp"
#include "ComponentEditor/MaterialEditor.hpp"
#include "ComponentEditor/DirectionalLightEditor.hpp"
#include "ComponentEditor/PointLightEditor.hpp"
#include "ComponentEditor/SpotLightEditor.hpp"
#include "ComponentEditor/ListenerEditor.hpp"
#include "ComponentEditor/SoundSourceEditor.hpp"
#include "ComponentEditor/ParticleEmitterEditor.hpp"
#include "../Label.hpp"
#include "StringEditor.hpp"
#include <Engine/Entity/Entity.hpp>
#include "../ImageTextButton.hpp"
#include <Engine/Util/Input.hpp>
#include <imgui.h>

using namespace GUI;

EntityEditor::EntityEditor() {
    /*rectangle = Managers().resourceManager->CreateRectangle();
    
    font = Managers().resourceManager->CreateFontEmbedded(ABEEZEE_TTF, ABEEZEE_TTF_LENGTH, 16.f);
    nameLabel = new Label(this, font, "Name");
    nameEditor = new StringEditor(this, font);
    
    addComponentTexture = Managers().resourceManager->CreateTexture2D(ADD_PNG, ADD_PNG_LENGTH);
    addComponentButton = new ImageTextButton(this, addComponentTexture, font, "Add component");
    addComponentButton->SetImageSize(glm::vec2(addComponentTexture->GetWidth(), addComponentTexture->GetHeight()));
    addComponentButton->SetClickedCallback(std::bind(&AddComponentPressed, this));
    
    removeEntityTexture = Managers().resourceManager->CreateTexture2D(SUBTRACT_PNG, SUBTRACT_PNG_LENGTH);
    removeEntityButton = new ImageTextButton(this, removeEntityTexture, font, "Delete entity");
    removeEntityButton->SetImageSize(glm::vec2(removeEntityTexture->GetWidth(), removeEntityTexture->GetHeight()));
    removeEntityButton->SetClickedCallback(std::bind(&RemoveEntityPressed, this));
    
    editors.push_back(new TransformEditor(this));
    editors.push_back(new PhysicsEditor(this));
    editors.push_back(new LensEditor(this));
    editors.push_back(new MeshEditor(this, modelSelector));
    editors.push_back(new MaterialEditor(this, textureSelector));
    editors.push_back(new DirectionalLightEditor(this));
    editors.push_back(new PointLightEditor(this));
    editors.push_back(new SpotLightEditor(this));
    editors.push_back(new ListenerEditor(this));
    editors.push_back(new SoundSourceEditor(this, soundSelector));
    editors.push_back(new ParticleEmitterEditor(this));
    
    this->componentAdder = componentAdder;
    
    SetVisible(false);*/
}

EntityEditor::~EntityEditor() {
    /*Managers().resourceManager->FreeRectangle();
    Managers().resourceManager->FreeFont(font);
    
    delete nameLabel;
    delete nameEditor;
    
    Managers().resourceManager->FreeTexture2D(addComponentTexture);
    delete addComponentButton;
    
    Managers().resourceManager->FreeTexture2D(removeEntityTexture);
    delete removeEntityButton;
    
    for (ComponentEditor* editor : editors)
        delete editor;*/
}

/*void EntityEditor::Update() {
    nameEditor->Update();
    addComponentButton->Update();
    removeEntityButton->Update();
    
    if (Input()->ScrollUp() && scrollPosition > 0U) {
        --scrollPosition;
        SetPosition(GetPosition());
    } else if (Input()->ScrollDown()) {
        std::size_t visibleEditors = 0U;
        for (ComponentEditor* editor : editors) {
            if (editor->IsVisible())
                ++visibleEditors;
        }
        
        if (scrollPosition < visibleEditors - 1) {
            ++scrollPosition;
            SetPosition(GetPosition());
        }
    }
    
    std::size_t i = 0U;
    for (ComponentEditor* editor : editors) {
        if (editor->IsVisible() && i++ >= scrollPosition) {
            editor->Update();
        }
    }
}

void EntityEditor::Render() {
    glm::vec3 color(0.06666666666f, 0.06274509803f, 0.08235294117f);
    rectangle->Render(GetPosition(), size, color);
    
    nameLabel->Render();
    nameEditor->Render();
    addComponentButton->Render();
    removeEntityButton->Render();
    
    // Draw scrollbar.
    if (!editors.empty()) {
        color = glm::vec3(0.16078431372f, 0.15686274509f, 0.17647058823f);
        float yScrolled = 0.f;
        float yCovered = 0.f;
        float yTotal = 0.f;
        for (std::size_t i=0U; i < editors.size(); ++i) {
            if (editors[i]->IsVisible()) {
                yTotal += editors[i]->GetSize().y;
                
                if (i < scrollPosition) {
                    yScrolled += editors[i]->GetSize().y;
                } else if (yTotal - yScrolled < size.y) {
                    yCovered += editors[i]->GetSize().y;
                }
            }
        }
        rectangle->Render(GetPosition() + glm::vec2(size.x - 20.f, size.y * yScrolled / yTotal), glm::vec2(20.f, size.y * yCovered / yTotal), color);
    }
    
    std::size_t i = 0U;
    for (ComponentEditor* editor : editors) {
        if (editor->IsVisible() && i++ >= scrollPosition) {
            editor->Render();
        }
    }
}*/

void EntityEditor::Show() {
    if (ImGui::Begin(("Entity: " + entity->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(entity))).c_str(), &visible)) {
        ImGui::InputText("Name", name, 128);
        entity->name = name;
    }
    ImGui::End();
}

void EntityEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    
    strcpy(name, entity->name.c_str());
}

bool EntityEditor::IsVisible() const {
    return visible;
}

void EntityEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void EntityEditor::AddComponentPressed() {
    componentAdder->SetEntity(entity);
    componentAdder->SetComponentAddedCallback(std::bind(&SetEntity, this, entity));
    componentAdder->SetVisible(true);
}
