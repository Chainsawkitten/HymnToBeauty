#include "ModelEditor.hpp"

#include <Engine/Geometry/Model.hpp>
#include "../FileSelector.hpp"
#include <functional>
#include <Engine/Hymn.hpp>
#include <Engine/Util/FileSystem.hpp>
#include <imgui.h>
#include "../../Util/AssetConverter.hpp"
#include "../../Util/AssetConverterSkeleton.hpp"
#include "../../Util/AssetMetaData.hpp"
#include <Utility/Log.hpp>
#include <Engine/Animation/AnimationClip.hpp>
#include <Engine/Animation/Skeleton.hpp>
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <Engine/Entity/Entity.hpp>
#include <Engine/Component/Mesh.hpp>
#include <Engine/Component/Material.hpp>

using namespace GUI;

ModelEditor::ModelEditor() {
    name[0] = '\0';
}

void ModelEditor::Show() {
    if (ImGui::Begin(("Model: " + model->name + "###" + std::to_string(reinterpret_cast<uintptr_t>(model))).c_str(), &visible, ImGuiWindowFlags_NoResize)) {
        if (ImGui::InputText("Name", name, 128, ImGuiInputTextFlags_EnterReturnsTrue)) {
            model->name = name;

            // Rename all the files.
            if (FileSystem::FileExists((destination + ".fbx").c_str())) {
                std::string newDestination = Hymn().GetPath() + "/" + model->path + name;

                rename((destination + ".fbx").c_str(), (newDestination + ".fbx").c_str());

                if (FileSystem::FileExists((destination + ".asset").c_str()))
                    rename((destination + ".asset").c_str(), (newDestination + ".asset").c_str());

                if (FileSystem::FileExists((destination + ".asset.meta").c_str()))
                    rename((destination + ".asset.meta").c_str(), (newDestination + ".asset.meta").c_str());

                destination = newDestination;
            }
        }

        if (ImGui::Button("Open model")) {
            // Currently only fbx is tested.
            fileSelector.AddExtensions("fbx");
            fileSelector.SetInitialPath(Hymn().GetPath().c_str());
            fileSelector.SetFileSelectedCallback(std::bind(&ModelEditor::FileSelected, this, std::placeholders::_1));
            fileSelector.SetVisible(true);
        }

        if (hasSourceFile) {
            ImGui::Text("Mesh data");
            ImGui::Checkbox("Uniform scaling", &uniformScaling);

            if (uniformScaling) {
                float uniScale = scale.x;
                ImGui::DragFloat("Scale", &uniScale, 0.01f);
                scale = glm::vec3(uniScale);
            } else
                ImGui::DragFloat3("Scale", &scale[0], 0.01f);

            ImGui::Checkbox("Triangulate", &triangulate);
            ImGui::Checkbox("Import normals", &importNormals);
            ImGui::Checkbox("Import tangents", &importTangents);
            ImGui::Checkbox("Import textures", &importTextures);
            ImGui::Checkbox("Flip UVs", &flipUVs);
            ImGui::Checkbox("Create scene", &createScene);
            ImGui::Checkbox("CPU", &CPU);
            ImGui::Checkbox("GPU", &GPU);

            std::string button = isImported ? "Re-import" : "Import";

            if (ImGui::Button(button.c_str())) {
                AssetConverter::Material materials;

                // Convert to .asset format.
                AssetConverter asset;
                asset.Convert(source.c_str(), (destination + ".asset").c_str(), scale, triangulate, importNormals, importTangents, flipUVs, importTextures, materials, CPU, GPU);
                model->Load((destination + ".asset").c_str());
                msgString = asset.Success() ? "Success\n" : asset.GetErrorString();
                isImported = true;

                // Generate meta data.
                AssetMetaData::MeshMetaData metaData;
                metaData.triangulate = triangulate;
                metaData.importNormals = importNormals;
                metaData.importTangents = importTangents;
                metaData.CPU = CPU;
                metaData.GPU = GPU;
                AssetMetaData::SaveMetaData((destination + ".asset.meta").c_str(), &metaData);

                // Import textures.
                TextureAsset* albedo = nullptr;
                TextureAsset* normal = nullptr;
                TextureAsset* roughness = nullptr;
                TextureAsset* metallic = nullptr;
                if (importTextures) {
                    albedo = LoadTexture(materials.albedo, "Albedo");
                    normal = LoadTexture(materials.normal, "Normal");
                    roughness = LoadTexture(materials.roughness, "Roughness");
                    metallic = LoadTexture(materials.metallic, "Metallic");
                }

                // Create scene containing an entity with the model and textures.
                if (createScene) {
                    // Create resource.
                    ResourceList::Resource resource;
                    resource.type = ResourceList::Resource::SCENE;
                    resource.scene = new std::string(model->name + "Scene");
                    folder->resources.push_back(resource);

                    // Create and save scene.
                    World* world = new World();
                    world->CreateRoot();
                    Entity* root = world->GetRoot();

                    // Unique identifiers are based on the current time.
                    // Since we're creating two entities at the same time, we fake that
                    // the root was created one second ago to avoid duplicate UIDs.
                    root->SetUniqueIdentifier(root->GetUniqueIdentifier() - 1);

                    Entity* entity = root->AddChild(model->name);

                    Component::Mesh* mesh = entity->AddComponent<Component::Mesh>();
                    mesh->geometry = model;

                    Component::Material* material = entity->AddComponent<Component::Material>();
                    if (albedo != nullptr)
                        material->albedo = albedo;
                    if (normal != nullptr)
                        material->normal = normal;
                    if (roughness != nullptr)
                        material->roughness = roughness;
                    if (metallic != nullptr)
                        material->metallic = metallic;

                    world->Save(Hymn().GetPath() + "/" + model->path + model->name + "Scene.json");

                    // Cleanup.
                    mesh->geometry = nullptr;
                    mesh->Kill();
                    material->albedo = nullptr;
                    material->normal = nullptr;
                    material->roughness = nullptr;
                    material->metallic = nullptr;
                    material->Kill();
                    delete world;
                }
            }

            if (isImported)
                ImGui::Text(msgString.c_str());
        }
    }
    ImGui::End();

    if (fileSelector.IsVisible())
        fileSelector.Show();
}

const Geometry::Model* ModelEditor::GetModel() const {
    return model;
}

void ModelEditor::SetModel(ResourceList::ResourceFolder* folder, Geometry::Model* model) {
    this->folder = folder;
    this->model = model;

    strcpy(name, model->name.c_str());

    destination = Hymn().GetPath() + "/" + model->path + name;

    RefreshImportSettings();
}

bool ModelEditor::IsVisible() const {
    return visible;
}

void ModelEditor::SetVisible(bool visible) {
    this->visible = visible;
}

void ModelEditor::FileSelected(const std::string& file) {
    std::string name = FileSystem::GetName(file).c_str();

    source = file;

    // Rename the model to the name of the source file.
    strcpy(this->name, name.c_str());
    model->name = this->name;

    destination = Hymn().GetPath() + "/" + model->path + name;

    // Check if source file is in proper directory, otherwise, copy it.
    if (!FileSystem::FileExists((destination + "." + FileSystem::GetExtension(source)).c_str()))
        FileSystem::Copy(source.c_str(), (destination + "." + FileSystem::GetExtension(source)).c_str());

    hasSourceFile = true;

    RefreshImportSettings();
}

void ModelEditor::RefreshImportSettings() {
    // Check if the source file exist, currently only .fbx is supported.
    if (!FileSystem::FileExists((destination + ".fbx").c_str())) {
        hasSourceFile = false;
        isImported = false;
        return;
    }

    // Check if meta file already exists. If it does import the metadata.
    std::string filePath(destination + ".asset.meta");
    if (FileSystem::FileExists(filePath.c_str())) {
        AssetMetaData::MeshMetaData metaData;
        AssetMetaData::LoadMetaData(filePath.c_str(), &metaData);
        triangulate = metaData.triangulate;
        importNormals = metaData.importNormals;
        importTangents = metaData.importTangents;
        CPU = metaData.CPU;
        GPU = metaData.GPU;

        isImported = true;
    } else
        isImported = false;
}

TextureAsset* ModelEditor::LoadTexture(const std::string& path, const std::string& name) {
    if (!path.empty()) {
        std::string textureName = model->name + name;
        std::string src = FileSystem::GetDirectory(source) + path;
        std::string dest = model->path + textureName;

        // Copy file.
        FileSystem::Copy(src.c_str(), (Hymn().GetPath() + "/" + dest + ".png").c_str());

        // Add texture asset.
        ResourceList::Resource resource;
        resource.type = ResourceList::Resource::TEXTURE;
        resource.texture = Managers().resourceManager->CreateTextureAsset(dest);
        folder->resources.push_back(resource);

        return resource.texture;
    }

    return nullptr;
}
