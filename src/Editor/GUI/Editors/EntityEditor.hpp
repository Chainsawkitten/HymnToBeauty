#pragma once

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <Engine/Entity/Entity.hpp>
#include <imgui.h>
#include "../ResourceSelector.hpp"

namespace Component {
class Mesh;
class Camera;
class Material;
class DirectionalLight;
class PointLight;
class SpotLight;
class Listener;
class RigidBody;
class Script;
class Shape;
class SoundSource;
class Sprite;
class Trigger;
} // namespace Component

namespace GUI {
class IShapeEditor;
class RigidBodyEditor;
class TriggerEditor;

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

    /// Get the entity being edited
    /**
     * @return The Entity object being edited.
     */
    Entity* GetEntity();

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
    template <typename type> void AddEditor(const std::string& name, std::function<void(type*)> editorFunction);
    template <typename type> void AddComponent(const std::string& name);
    template <typename type> void EditComponent(const std::string& name, std::function<void(type*)> editorFunction);

    // Editors
    void MeshEditor(Component::Mesh* mesh);
    void CameraEditor(Component::Camera* camera);
    void MaterialEditor(Component::Material* material);
    void DirectionalLightEditor(Component::DirectionalLight* directionalLight);
    void PointLightEditor(Component::PointLight* pointLight);
    void SpotLightEditor(Component::SpotLight* spotLight);
    void ListenerEditor(Component::Listener* listener);
    void RigidBodyEditor(Component::RigidBody* rigidBody);
    void ScriptEditor(Component::Script* script);
    void ShapeEditor(Component::Shape* shape);
    void SoundSourceEditor(Component::SoundSource* soundSource);
    void SpriteEditor(Component::Sprite* sprite);
    void TriggerEditor(Component::Trigger* trigger);

    Entity* entity = nullptr;
    bool visible = false;
    char name[128];
    char stringPropertyBuffer[128];
    glm::vec3 position;
    glm::vec3 scale;

    struct Editor {
        std::function<void()> addFunction;
        std::function<void()> editFunction;
    };
    std::vector<Editor> editors;
    std::vector<IShapeEditor*> shapeEditors;
    int selectedShape = -1;

    std::unique_ptr<GUI::RigidBodyEditor> rigidBodyEditor;

    ResourceSelector resourceSelector;
    std::unique_ptr<GUI::TriggerEditor> triggerEditor;

    bool albedoShow = false;
    bool normalShow = false;
    bool roughnessMetallicShow = false;

    bool textureShow = false;

    bool cameraLayers[32];
    bool meshLayers[32];
    bool spriteLayers[32];
};
} // namespace GUI

template <typename type> void GUI::EntityEditor::AddEditor(const std::string& name, std::function<void(type*)> editorFunction) {
    Editor editor;
    editor.addFunction = std::bind(&EntityEditor::AddComponent<type>, this, name);
    editor.editFunction = std::bind(&EntityEditor::EditComponent<type>, this, name, editorFunction);
    editors.push_back(editor);
}

template <typename type> void GUI::EntityEditor::AddComponent(const std::string& name) {
    if (entity->GetComponent<type>() == nullptr) {
        if (ImGui::Selectable(name.c_str())) {
            entity->AddComponent<type>();
            SetEntity(entity);
        }
    }
}

template <typename type> void GUI::EntityEditor::EditComponent(const std::string& name, std::function<void(type*)> editorFunction) {
    type* component = entity->GetComponent<type>();
    if (component != nullptr && ImGui::CollapsingHeader(name.c_str())) {
        ImGui::PushID(name.c_str());

        editorFunction(component);

        if (ImGui::Button("Remove"))
            entity->KillComponent<type>();

        ImGui::PopID();
    }
}
