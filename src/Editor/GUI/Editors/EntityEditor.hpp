#pragma once

#include <vector>
#include <string>
#include <functional>
#include <Engine/Entity/Entity.hpp>
#include <imgui.h>

namespace Component {
    class Transform;
    class Physics;
}

namespace GUI {
    /// Used to edit an entity.
    class EntityEditor {
        public:
            /// Create new entity editor.
            EntityEditor();
            
            /// Destructor.
            ~EntityEditor();
            
            /// Show the editor.
            void Show();
            
            /// Set the entity to edit.
            /**
             * @param entity The entity to edit.
             */
            void SetEntity(Entity* entity);
            
            /// Get whether the window is visible.
            /**
             * @return Whether the window is visible.
             */
            bool IsVisible() const;
            
            /// Set whether the window should be visible.
            /**
             * @param visible Whether the window should be visible.
             */
            void SetVisible(bool visible);
            
        private:
            template<typename type> void AddEditor(const std::string& name, std::function<void(type*)> editorFunction);
            template<typename type> void AddComponent(const std::string& name);
            template<typename type> void EditComponent(const std::string& name, std::function<void(type*)> editorFunction);
            
            // Editors
            void TransformEditor(Component::Transform* transform);
            void PhysicsEditor(Component::Physics* physics);
            
            Entity* entity = nullptr;
            bool visible = false;
            char name[128] = "";
            
            struct Editor {
                std::function<void()> addFunction;
                std::function<void()> editFunction;
            };
            std::vector<Editor> editors;
    };
}

template<typename type> void GUI::EntityEditor::AddEditor(const std::string& name, std::function<void(type*)> editorFunction) {
    Editor editor;
    editor.addFunction = std::bind(&AddComponent<type>, this, name);
    editor.editFunction = std::bind(&EditComponent<type>, this, name, editorFunction);
    editors.push_back(editor);
}

template<typename type> void GUI::EntityEditor::AddComponent(const std::string& name) {
    if (entity->GetComponent<type>() == nullptr)
        if (ImGui::Selectable(name.c_str()))
            entity->AddComponent<type>();
}

template<typename type> void GUI::EntityEditor::EditComponent(const std::string& name, std::function<void(type*)> editorFunction) {
    type* component = entity->GetComponent<type>();
    if (component != nullptr && ImGui::CollapsingHeader(name.c_str())) {
        editorFunction(component);
    }
}
