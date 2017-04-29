#pragma once

#include <vector>
#include <string>
#include <functional>
#include <Engine/Entity/Entity.hpp>
#include <imgui.h>

namespace Component {
    class Animation;
    class Physics;
    class Mesh;
    class Lens;
    class Material;
    class DirectionalLight;
    class PointLight;
    class SpotLight;
    class Listener;
    class Script;
    class SoundSource;
    class ParticleEmitter;
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

            /// Checks if the editor is showing this entity.
            /**
             * @param entity The entity to check.
             * @return Is it showing.
             */
            bool ShowsEntity(Entity* entity);

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
            // TEMPORARY
            ImVec2 foo[10];
            template<typename type> void AddEditor(const std::string& name, std::function<void(type*)> editorFunction);
            template<typename type> void AddComponent(const std::string& name);
            template<typename type> void EditComponent(const std::string& name, std::function<void(type*)> editorFunction);
            
            // Editors
            void AnimationEditor(Component::Animation* animation);
            void PhysicsEditor(Component::Physics* physics);
            void MeshEditor(Component::Mesh* mesh);
            void LensEditor(Component::Lens* lens);
            void MaterialEditor(Component::Material* material);
            void DirectionalLightEditor(Component::DirectionalLight* directionalLight);
            void PointLightEditor(Component::PointLight* pointLight);
            void SpotLightEditor(Component::SpotLight* spotLight);
            void ListenerEditor(Component::Listener* listener);
            void ScriptEditor(Component::Script* script);
            void SoundSourceEditor(Component::SoundSource* soundSource);
            void ParticleEmitterEditor(Component::ParticleEmitter* particleEmitter);
            
            Entity* entity = nullptr;
            bool visible = false;
            char name[128];
            
            struct Editor {
                std::function<void()> addFunction;
                std::function<void()> editFunction;
            };
            std::vector<Editor> editors;
    };
}

template<typename type> void GUI::EntityEditor::AddEditor(const std::string& name, std::function<void(type*)> editorFunction) {
    Editor editor;
    editor.addFunction = std::bind(&EntityEditor::AddComponent<type>, this, name);
    editor.editFunction = std::bind(&EntityEditor::EditComponent<type>, this, name, editorFunction);
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
        
        if (ImGui::Button("Remove"))
            entity->KillComponent<type>();
    }
}
