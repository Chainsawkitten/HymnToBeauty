#include "ListenerEditor.hpp"

#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Font/Font.hpp>
#include "ABeeZee.ttf.hpp"

#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Listener.hpp>

using namespace GUI;

ListenerEditor::ListenerEditor(Widget* parent) : ComponentEditor(parent, "Listener") {
    SetComponent<Component::Listener>();
    
    SetVisible(false);
}

void ListenerEditor::SetEntity(Entity* entity) {
    ComponentEditor::SetEntity(entity);
    
    if (entity == nullptr) {
        SetVisible(false);
    } else {
        Component::Listener* listener = entity->GetComponent<Component::Listener>();
        SetVisible(listener != nullptr);
    }
}
