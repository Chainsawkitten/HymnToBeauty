#include "EntityEditor.hpp"

#include <Engine/Component/Mesh.hpp>
#include <Engine/Component/Camera.hpp>
#include <Engine/Component/Material.hpp>
#include <Engine/Component/DirectionalLight.hpp>
#include <Engine/Component/PointLight.hpp>
#include <Engine/Component/SpotLight.hpp>
#include <Engine/Component/Listener.hpp>
#include <Engine/Component/RigidBody.hpp>
#include <Engine/Component/Script.hpp>
#include <Engine/Component/Shape.hpp>
#include <Engine/Component/SoundSource.hpp>
#include <Engine/Component/Sprite.hpp>
#include <Engine/Component/Trigger.hpp>
#include <Engine/Geometry/Model.hpp>
#include <Engine/Texture/TextureAsset.hpp>
#include <Video/Texture/Texture2D.hpp>
#include <Engine/Audio/SoundFile.hpp>
#include <Engine/Audio/SoundBuffer.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ScriptManager.hpp>
#include <Engine/Manager/PhysicsManager.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Hymn.hpp>
#include <angelscript.h>
#include <Video/LowLevelRenderer/Interface/Texture.hpp>

#include "../../Util/EditorSettings.hpp"
#include "../FileSelector.hpp"
#include "../../ImGui/GuiHelpers.hpp"
#include "../../Resources.hpp"
#include <imgui_internal.h>
#include <imgui.h>
#include "BoxShapeEditor.hpp"
#include "CapsuleShapeEditor.hpp"
#include "ConeShapeEditor.hpp"
#include "CylinderShapeEditor.hpp"
#include "PlaneShapeEditor.hpp"
#include "RigidBodyEditor.hpp"
#include "SphereShapeEditor.hpp"
#include "TriggerEditor.hpp"

using namespace GUI;

EntityEditor::EntityEditor() {
    name[0] = '\0';

    AddEditor<Component::Mesh>("Mesh", std::bind(&EntityEditor::MeshEditor, this, std::placeholders::_1));
    AddEditor<Component::Camera>("Camera", std::bind(&EntityEditor::CameraEditor, this, std::placeholders::_1));
    AddEditor<Component::Material>("Material", std::bind(&EntityEditor::MaterialEditor, this, std::placeholders::_1));
    AddEditor<Component::DirectionalLight>("Directional light", std::bind(&EntityEditor::DirectionalLightEditor, this, std::placeholders::_1));
    AddEditor<Component::PointLight>("Point light", std::bind(&EntityEditor::PointLightEditor, this, std::placeholders::_1));
    AddEditor<Component::SpotLight>("Spot light", std::bind(&EntityEditor::SpotLightEditor, this, std::placeholders::_1));
    AddEditor<Component::Listener>("Listener", std::bind(&EntityEditor::ListenerEditor, this, std::placeholders::_1));
    AddEditor<Component::RigidBody>("Rigid body", std::bind(&EntityEditor::RigidBodyEditor, this, std::placeholders::_1));
    AddEditor<Component::Script>("Script", std::bind(&EntityEditor::ScriptEditor, this, std::placeholders::_1));
    AddEditor<Component::Shape>("Shape", std::bind(&EntityEditor::ShapeEditor, this, std::placeholders::_1));
    AddEditor<Component::SoundSource>("Sound source", std::bind(&EntityEditor::SoundSourceEditor, this, std::placeholders::_1));
    AddEditor<Component::Sprite>("Sprite", std::bind(&EntityEditor::SpriteEditor, this, std::placeholders::_1));
    AddEditor<Component::Trigger>("Trigger", std::bind(&EntityEditor::TriggerEditor, this, std::placeholders::_1));

    shapeEditors.push_back(new SphereShapeEditor());
    shapeEditors.push_back(new PlaneShapeEditor());
    shapeEditors.push_back(new BoxShapeEditor());
    shapeEditors.push_back(new CylinderShapeEditor());
    shapeEditors.push_back(new ConeShapeEditor());
    shapeEditors.push_back(new CapsuleShapeEditor());
    selectedShape = 0;

    rigidBodyEditor.reset(new GUI::RigidBodyEditor);
    triggerEditor.reset(new GUI::TriggerEditor);
}

EntityEditor::~EntityEditor() {
    for (auto shapeEditor : shapeEditors) {
        delete shapeEditor;
    }
}

void EntityEditor::Show() {
    if (ImGui::Begin(("Entity: " + entity->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(entity))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        ImGui::InputText("Name", name, 128);
        entity->name = name;
        ImGui::Text("Transform");
        ImGui::ShowHelpMarker("The entity's position, rotation and scale.", 75.f);
        ImGui::Indent();

        if (EditorSettings::GetInstance().GetBool("Grid Snap")) {
            int toNearest = EditorSettings::GetInstance().GetLong("Grid Snap Size");

            int value = static_cast<unsigned int>(entity->position.x);
            int rest = value % toNearest;

            if (rest > (toNearest / 2)) {
                entity->position.x = static_cast<float>((value - rest) + toNearest);
            } else {
                entity->position.x = static_cast<float>(value - rest);
            }

            value = static_cast<int>(entity->position.y);
            rest = value % toNearest;

            if (rest > (toNearest / 2)) {
                entity->position.y = static_cast<float>((value - rest) + toNearest);
            } else {
                entity->position.y = static_cast<float>((value - rest));
            }

            value = static_cast<int>(entity->position.z);
            rest = value % toNearest;

            if (rest > (toNearest / 2)) {
                entity->position.z = static_cast<float>((value - rest) + toNearest);
            } else {
                entity->position.z = static_cast<float>(value - rest);
            }
        }

        ImGui::DraggableVec3("Position", entity->position);

        glm::vec3 eulerAngles = glm::eulerAngles(entity->rotation);
        eulerAngles = glm::degrees(eulerAngles);
        if (ImGui::InputFloat3("Euler angles", &eulerAngles.x))
            entity->SetLocalOrientation(glm::quat(glm::radians(eulerAngles)));

        ImGui::DraggableVec3("Scale", entity->scale);
        ImGui::Text("Unique Identifier: %u", entity->GetUniqueIdentifier());
        ImGui::Unindent();
        if (!entity->IsScene()) {
            if (ImGui::Button("Add component"))
                ImGui::OpenPopup("Add component");

            if (ImGui::BeginPopup("Add component")) {
                ImGui::Text("Components");
                ImGui::Separator();

                for (Editor& editor : editors) {
                    editor.addFunction();
                }

                ImGui::EndPopup();
            }

            for (Editor& editor : editors) {
                editor.editFunction();
            }
        }
    }

    ImGui::End();
}

void EntityEditor::SetEntity(Entity* entity) {
    this->entity = entity;
    strcpy(name, entity->name.c_str());

    auto shapeComp = this->entity->GetComponent<Component::Shape>();
    if (shapeComp != nullptr) {
        Physics::Shape& shape = *shapeComp->GetShape();
        for (uint32_t i = 0; i < shapeEditors.size(); ++i) {
            if (shapeEditors[i]->SetFromShape(shape)) {
                selectedShape = i;
            }
        }
    }

    const Component::Camera* camera = entity->GetComponent<Component::Camera>();
    if (camera != nullptr) {
        for (uint32_t i = 0; i < 32; ++i) {
            cameraLayers[i] = (camera->layerMask & (1u << i)) != 0u;
        }
    }

    const Component::Mesh* mesh = entity->GetComponent<Component::Mesh>();
    if (mesh != nullptr) {
        for (uint32_t i = 0; i < 32; ++i) {
            meshLayers[i] = (mesh->layerMask & (1u << i)) != 0u;
        }
    }

    const Component::Sprite* sprite = entity->GetComponent<Component::Sprite>();
    if (sprite != nullptr) {
        for (uint32_t i = 0; i < 32; ++i) {
            spriteLayers[i] = (sprite->layerMask & (1u << i)) != 0u;
        }
    }
}

Entity* EntityEditor::GetEntity() {
    return entity;
}

bool EntityEditor::ShowsEntity(Entity* entity) {
    return this->entity == entity;
}

bool EntityEditor::IsVisible() const {
    return visible;
}

void EntityEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void EntityEditor::MeshEditor(Component::Mesh* mesh) {
    ImGui::Indent();
    if (ImGui::Button("Select model##Mesh"))
        ImGui::OpenPopup("Select model##Mesh");

    ImGui::Text("Layers");
    ImGui::Indent();
    mesh->layerMask = 0u;
    for (uint32_t i = 0; i < 32; ++i) {
        if (i % 8u != 0u)
            ImGui::SameLine();
        ImGui::Selectable(std::to_string(i + 1).c_str(), &meshLayers[i], 0, ImVec2(12, 12));
        mesh->layerMask |= meshLayers[i] ? (1u << i) : 0u;
    }
    ImGui::Unindent();

    if (ImGui::BeginPopup("Select model##Mesh")) {
        ImGui::Text("Models");
        ImGui::Separator();

        if (resourceSelector.Show(ResourceList::Resource::Type::MODEL)) {
            if (mesh->geometry != nullptr)
                Managers().resourceManager->FreeModel(dynamic_cast<Geometry::Model*>(mesh->geometry));

            mesh->geometry = Managers().resourceManager->CreateModel(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::EndPopup();
    }

    // Paint Mode. Load vertices and indices.
    if (entity->GetComponent<Component::Mesh>()->geometry != nullptr) {
        if (!entity->loadPaintModeClicked) {
            if (ImGui::Button("Load paint mode.")) {
                entity->loadPaintModeClicked = true;
                entity->vertsLoaded = true;
            }
        }

        if (entity->loadPaintModeClicked) {
            if (!entity->brushActive) {
                if (ImGui::Button("Activate paint brush")) {
                    entity->brushActive = true;
                }
            }

            if (entity->brushActive) {
                if (ImGui::Button("Exit paint brush")) {
                    entity->brushActive = false;
                    entity->loadPaintModeClicked = false;
                    entity->sceneChosen = false;
                }
            }
        }
    }
    ImGui::Unindent();
}

void EntityEditor::CameraEditor(Component::Camera* camera) {
    ImGui::Indent();
    ImGui::Checkbox("Orthographic", &camera->orthographic);
    if (camera->orthographic) {
        ImGui::DraggableFloat("Size", camera->size, 0.01f);
    } else {
        ImGui::DraggableFloat("Field of view", camera->fieldOfView, 0.0f, 180.f);
    }
    ImGui::DraggableFloat("Z near", camera->zNear, 0.0f);
    ImGui::DraggableFloat("Z far", camera->zFar, 0.0f);
    ImGui::InputFloat4("Viewport", &camera->viewport.x);
    ImGui::InputInt("Order", &camera->order);
    ImGui::Checkbox("Overlay", &camera->overlay);
    ImGui::Text("Layers");
    ImGui::Indent();
    camera->layerMask = 0u;
    for (uint32_t i = 0; i < 32; ++i) {
        if (i % 8u != 0u)
            ImGui::SameLine();
        ImGui::Selectable(std::to_string(i + 1).c_str(), &cameraLayers[i], 0, ImVec2(12, 12));
        camera->layerMask |= cameraLayers[i] ? (1u << i) : 0u;
    }
    ImGui::Unindent();

    if (ImGui::CollapsingHeader("Filters")) {
        ImGui::Indent();

        // Bloom
        if (ImGui::CollapsingHeader("Bloom")) {
            ImGui::TextWrapped("Causes bright areas of the image to bleed.");

            ImGui::Checkbox("Enable##Bloom", &camera->filterSettings.bloom);
            ImGui::DragFloat("Intensity", &camera->filterSettings.bloomIntensity, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Threshold", &camera->filterSettings.bloomThreshold, 0.01f, 0.0f, 1.0f);
            ImGui::DragFloat("Scatter", &camera->filterSettings.bloomScatter, 0.01f, 0.0f, 1.0f);
        }

        // Gamma
        if (ImGui::CollapsingHeader("Gamma correction")) {
            ImGui::TextWrapped("Gamma correction filter.");

            ImGui::DragFloat("Gamma", &camera->filterSettings.gamma, 0.1f, 1.0f, 10.0f);
        }

        // FXAA
        if (ImGui::CollapsingHeader("FXAA")) {
            ImGui::TextWrapped("Cheap anti-aliasing.");

            ImGui::Checkbox("Enable##FXAA", &camera->filterSettings.fxaa);
        }

        // Dither
        if (ImGui::CollapsingHeader("Dither")) {
            ImGui::TextWrapped("Prevents large-scale color patterns such as color banding.");

            ImGui::Checkbox("Enable##Dither", &camera->filterSettings.ditherApply);
        }

        ImGui::Unindent();
    }

    ImGui::Unindent();
}

void EntityEditor::MaterialEditor(Component::Material* material) {
    // Albedo
    ImGui::Text("Albedo");
    ImGui::Indent();
    if (material->albedo->GetTexture()->IsLoaded())
        ImGui::Image((void*)material->albedo->GetTexture()->GetTexture(), ImVec2(128, 128));

    if (ImGui::Button("Select albedo texture")) {
        albedoShow = true;
        normalShow = false;
        roughnessMetallicShow = false;
    }

    ImGui::Unindent();

    // Normal
    ImGui::Text("Normal");
    ImGui::Indent();
    if (material->normal->GetTexture()->IsLoaded())
        ImGui::Image((void*)material->normal->GetTexture()->GetTexture(), ImVec2(128, 128));

    if (ImGui::Button("Select normal texture")) {
        albedoShow = false;
        normalShow = true;
        roughnessMetallicShow = false;
    }

    ImGui::Unindent();

    ImGui::Unindent();

    // Roughness
    ImGui::Text("Roughness-metallic");
    ImGui::Indent();
    if (material->roughnessMetallic->GetTexture()->IsLoaded())
        ImGui::Image((void*)material->roughnessMetallic->GetTexture()->GetTexture(), ImVec2(128, 128));

    if (ImGui::Button("Select roughness texture")) {
        albedoShow = false;
        normalShow = false;
        roughnessMetallicShow = true;
    }

    ImGui::Unindent();

    // Select albedo.
    if (albedoShow) {
        ImGui::Begin("Textures", &albedoShow);
        if (resourceSelector.Show(ResourceList::Resource::Type::TEXTURE)) {
            if (material->albedo != Managers().resourceManager->GetDefaultAlbedo())
                Managers().resourceManager->FreeTextureAsset(material->albedo);

            material->albedo = Managers().resourceManager->CreateTextureAsset(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::End();
    }

    // Select normal.
    if (normalShow) {
        ImGui::Begin("Textures", &normalShow);
        if (resourceSelector.Show(ResourceList::Resource::Type::TEXTURE)) {
            if (material->normal != Managers().resourceManager->GetDefaultNormal())
                Managers().resourceManager->FreeTextureAsset(material->normal);

            material->normal = Managers().resourceManager->CreateTextureAsset(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::End();
    }

    // Select roughness.
    if (roughnessMetallicShow) {
        ImGui::Begin("Textures", &roughnessMetallicShow);
        if (resourceSelector.Show(ResourceList::Resource::Type::TEXTURE)) {
            if (material->roughnessMetallic != Managers().resourceManager->GetDefaultRoughnessMetallic())
                Managers().resourceManager->FreeTextureAsset(material->roughnessMetallic);

            material->roughnessMetallic = Managers().resourceManager->CreateTextureAsset(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::End();
    }
}

void EntityEditor::DirectionalLightEditor(Component::DirectionalLight* directionalLight) {
    ImGui::Indent();
    ImGui::ColorEdit3("Color", &directionalLight->color[0]);
    ImGui::DraggableFloat("Ambient coefficient", directionalLight->ambientCoefficient, 0.0f);
    ImGui::Unindent();
}

void EntityEditor::PointLightEditor(Component::PointLight* pointLight) {
    ImGui::Indent();
    ImGui::ColorEdit3("Color", &pointLight->color[0]);
    ImGui::DraggableFloat("Attenuation", pointLight->attenuation, 0.0f);
    ImGui::DraggableFloat("Intensity", pointLight->intensity, 0.0f);
    ImGui::DraggableFloat("Distance", pointLight->distance, 0.0f);
    ImGui::Unindent();
}

void EntityEditor::SpotLightEditor(Component::SpotLight* spotLight) {
    ImGui::Indent();
    ImGui::ColorEdit3("Color", &spotLight->color[0]);
    ImGui::DraggableFloat("Ambient coefficient", spotLight->ambientCoefficient, 0.0f);
    ImGui::DraggableFloat("Attenuation", spotLight->attenuation, 0.0f);
    ImGui::DraggableFloat("Intensity", spotLight->intensity, 0.0f);
    ImGui::DraggableFloat("Cone angle", spotLight->coneAngle, 0.0f, 180.f);
    ImGui::DraggableFloat("Distance", spotLight->distance, 0.0f);
    ImGui::Unindent();
}

void EntityEditor::ListenerEditor(Component::Listener* listener) {}

void EntityEditor::RigidBodyEditor(Component::RigidBody* rigidBody) {
    rigidBodyEditor->Show(rigidBody);
}

void EntityEditor::ScriptEditor(Component::Script* script) {
    ImGui::Indent();

    if (ImGui::Button("Select script"))
        ImGui::OpenPopup("Select script");

    if (ImGui::BeginPopup("Select script")) {
        ImGui::Text("Scripts");
        ImGui::Separator();

        for (ScriptFile* scriptFile : Hymn().scripts) {
            if (ImGui::Selectable(scriptFile->name.c_str()))
                script->scriptFile = scriptFile;
        }

        ImGui::EndPopup();
    }

    if (script->scriptFile != nullptr) {
        ImGui::Text(script->scriptFile->name.c_str());
        ImGui::Separator();

        if (ImGui::Button("Fetch properties"))
            Managers().scriptManager->FillPropertyMap(script);

        if (script->instance != nullptr) {
            int propertyCount = script->instance->GetPropertyCount();

            for (int n = 0; n < propertyCount; n++) {
                std::string propertyName = script->instance->GetPropertyName(n);

                std::string popupName = "Add entity reference##";
                popupName += propertyName;

                int typeId = script->instance->GetPropertyTypeId(n);

                if (typeId == asTYPEID_INT32)
                    ImGui::InputInt(script->instance->GetPropertyName(n), (int*)script->GetDataFromPropertyMap(propertyName), 0);
                else if (typeId == asTYPEID_FLOAT)
                    ImGui::DraggableFloat(script->instance->GetPropertyName(n), *(float*)script->GetDataFromPropertyMap(propertyName), 0.0f);
                else if (typeId == script->instance->GetEngine()->GetTypeIdByDecl("Entity@")) {
                    if (propertyName != "self") {
                        unsigned int GUID = *(unsigned int*)script->GetDataFromPropertyMap(propertyName);

                        if (GUID != 0) {
                            std::string entityGUID = std::to_string(GUID);
                            std::string entityName = Hymn().GetEntityByGUID(GUID)->name;
                            std::string propertyText;
                            propertyText.reserve(propertyName.length() + entityName.length() + entityGUID.length() + 4); // additional `:  ()`
                            propertyText.append(propertyName).append(": ").append(entityName).append("(").append(entityGUID).append(")");

                            ImGui::Separator();

                            // Choosing other entity references
                            ImGui::Text(propertyText.c_str());

                            if (ImGui::Button(popupName.c_str()))
                                ImGui::OpenPopup(popupName.c_str());
                        } else {
                            std::string entityGUID = "???";
                            std::string entityName = "Uninitialized";
                            std::string propertyText;
                            propertyText.reserve(propertyName.length() + entityName.length() + entityGUID.length() + 4); // additional `:  ()`
                            propertyText.append(propertyName).append(": ").append(entityName).append("(").append(entityGUID).append(")");

                            ImGui::Separator();

                            // Choosing other entity references
                            ImGui::Text(propertyText.c_str());

                            if (ImGui::Button(popupName.c_str()))
                                ImGui::OpenPopup(popupName.c_str());
                        }

                        if (ImGui::BeginPopup(popupName.c_str())) {
                            ImGui::Text("Entities");
                            ImGui::Separator();
                            for (Entity* entity : Hymn().world.GetEntities()) /// @todo Change into a prettier tree structure or something, later.
                                if (ImGui::Selectable(entity->name.c_str()))
                                    *(unsigned int*)script->GetDataFromPropertyMap(propertyName) = entity->GetUniqueIdentifier();

                            ImGui::EndPopup();
                        }

                        ImGui::Separator();
                    }
                }
                /// @todo This will be used to handle objects in the scripts
                // else if (typeId & asTYPEID_SCRIPTOBJECT){
                //    asIScriptObject *obj = (asIScriptObject*)varPointer;
                //}
            }

            if (ImGui::Button("Reset properties")) {
                script->ClearPropertyMap();
                Managers().scriptManager->FillPropertyMap(script);
            }
        }
    } else
        ImGui::Text("No script loaded");

    ImGui::Unindent();
}

void EntityEditor::ShapeEditor(Component::Shape* shape) {
    if (ImGui::Combo("Shape", &selectedShape,
                     [](void* data, int idx, const char** outText) -> bool {
                         IShapeEditor* editor = *(reinterpret_cast<IShapeEditor**>(data) + idx);
                         *outText = editor->Label();
                         return true;
                     },
                     shapeEditors.data(), static_cast<int>(shapeEditors.size()))) {
        shapeEditors[selectedShape]->Apply(shape);
    }

    if (selectedShape != -1) {
        shapeEditors[selectedShape]->Show(shape);
    }
}

void EntityEditor::SpriteEditor(Component::Sprite* sprite) {
    ImGui::Text("Sprite");
    ImGui::Indent();

    if (sprite->texture->GetTexture()->IsLoaded())
        ImGui::Image((void*)sprite->texture->GetTexture()->GetTexture(), ImVec2(128, 128));

    if (ImGui::Button("Select texture")) {
        textureShow = true;
    }

    ImGui::DraggableFloat("Pixels per unit", sprite->pixelsPerUnit);
    ImGui::DraggableVec2("Pivot", sprite->pivot, 0.0f, 1.0f);
    ImGui::ColorEdit3("Tint", &sprite->tint[0]);
    ImGui::DraggableFloat("Alpha", sprite->alpha, 0.0f, 1.0f);
    ImGui::Text("Layers");
    ImGui::Indent();
    sprite->layerMask = 0u;
    for (uint32_t i = 0; i < 32; ++i) {
        if (i % 8u != 0u)
            ImGui::SameLine();
        ImGui::Selectable(std::to_string(i + 1).c_str(), &spriteLayers[i], 0, ImVec2(12, 12));
        sprite->layerMask |= spriteLayers[i] ? (1u << i) : 0u;
    }
    ImGui::Unindent();

    ImGui::Unindent();

    // Select texture.
    if (textureShow) {
        ImGui::Begin("Textures", &textureShow);
        if (resourceSelector.Show(ResourceList::Resource::Type::TEXTURE)) {
            if (sprite->texture != Managers().resourceManager->GetDefaultAlbedo())
                Managers().resourceManager->FreeTextureAsset(sprite->texture);

            sprite->texture = Managers().resourceManager->CreateTextureAsset(resourceSelector.GetSelectedResource().GetPath());
        }
        ImGui::End();
    }
}

void EntityEditor::SoundSourceEditor(Component::SoundSource* soundSource) {
    ImGui::Text("Sound");
    ImGui::Indent();

    if (soundSource->soundBuffer->GetSoundFile()) {
        ImGui::Text(soundSource->soundBuffer->GetSoundFile()->name.c_str());
    }

    if (ImGui::Button("Select sound"))
        ImGui::OpenPopup("Select sound");

    if (ImGui::BeginPopup("Select sound")) {
        ImGui::Text("Sounds");
        ImGui::Separator();

        if (resourceSelector.Show(ResourceList::Resource::Type::SOUND)) {
            Audio::SoundFile* soundFile = soundSource->soundBuffer->GetSoundFile();
            if (soundFile)
                Managers().resourceManager->FreeSound(soundFile);

            soundFile = Managers().resourceManager->CreateSound(resourceSelector.GetSelectedResource().GetPath());
            soundSource->soundBuffer->SetSoundFile(soundFile);
        }

        ImGui::EndPopup();
    }
    ImGui::Unindent();
    ImGui::Text("Sound properties");
    ImGui::Indent();
    ImGui::DraggableFloat("Pitch", soundSource->pitch, 0.0f);
    ImGui::DraggableFloat("Gain", soundSource->gain, 0.0f);
    ImGui::Checkbox("Loop", &soundSource->loop);
    ImGui::Unindent();
}

void EntityEditor::TriggerEditor(Component::Trigger* trigger) {
    ImGui::Indent();

    if (ImGui::Button("Edit"))
        triggerEditor->Open();

    triggerEditor->Show(*trigger);
    ImGui::Unindent();
}
