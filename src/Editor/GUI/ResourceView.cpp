#include "ResourceView.hpp"

#include <Engine/Animation/AnimationClip.hpp>
#include <Engine/Animation/AnimationController.hpp>
#include <Engine/Animation/Skeleton.hpp>
#include <Engine/Geometry/Model.hpp>
#include <Engine/Texture/TextureAsset.hpp>
#include <Engine/Audio/VorbisFile.hpp>
#include <Engine/Script/ScriptFile.hpp>
#include <Engine/Hymn.hpp>
#include <Engine/MainWindow.hpp>
#include <fstream>
#include <imgui.h>
#include "../ImGui/Splitter.hpp"
#include <Engine/Manager/Managers.hpp>
#include <Engine/Manager/ResourceManager.hpp>
#include <cstdio>
#include <Utility/Log.hpp>

// Fix windows.h pollution.
#ifdef _WIN32
#undef CreateDirectory
#endif

#include <Engine/Util/FileSystem.hpp>

using namespace GUI;
using namespace std;

extern std::string DefaultScriptBody;

ResourceView::ResourceView() {
    folderNameWindow.SetClosedCallback(std::bind(&ResourceView::FileNameWindowClosed, this, placeholders::_1));
    savePromptWindow.SetTitle("Save before you switch scene?");
    savePromptWindow.ResetDecision();
}

void ResourceView::Show() {
    ImVec2 size(MainWindow::GetInstance()->GetSize().x, MainWindow::GetInstance()->GetSize().y);

    // Splitter.
    ImGui::VerticalSplitter(ImVec2(static_cast<float>(sceneWidth), static_cast<float>(size.y - resourceHeight)), static_cast<int>(size.x - sceneWidth - editorWidth), splitterSize, resourceHeight, resourceResize, 20, static_cast<int>(size.y - 20));
    if (resourceResize)
        resourceHeight = static_cast<int>(size.y - resourceHeight);

    ImGui::SetNextWindowPos(ImVec2(static_cast<float>(sceneWidth), static_cast<float>(size.y - resourceHeight)));
    ImGui::SetNextWindowSize(ImVec2(static_cast<float>(size.x - sceneWidth - editorWidth), static_cast<float>(resourceHeight)));

    ImGui::Begin("Resources", nullptr, ImGuiWindowFlags_NoResize);

    // Show resources.
    animationClipPressed = false;
    animationControllerPressed = false;
    skeletonPressed = false;
    scriptPressed = false;
    texturePressed = false;
    modelPressed = false;
    soundPressed = false;

    ShowResourceFolder(Resources().resourceFolder, Resources().resourceFolder.name);

    // Change scene.
    if (changeScene) {
        if (Hymn().GetPath() != "") {
            savePromptWindow.SetVisible(true);
            savePromptWindow.Show();

            switch (savePromptWindow.GetDecision()) {
            case 0:
                sceneEditor.Save();
                sceneEditor.SetVisible(true);
                sceneEditor.SetScene(resourcePath, scene);
                Resources().activeScene = resourcePath + "/" + *scene;
                sceneEditor.entityEditor.SetVisible(false);
                Hymn().world.Clear();
                Hymn().world.Load(Hymn().GetPath() + "/" + Resources().activeScene + ".json");
                changeScene = false;
                savePromptWindow.SetVisible(false);
                savePromptWindow.ResetDecision();
                break;

            case 1:
                sceneEditor.SetVisible(true);
                sceneEditor.SetScene(resourcePath, scene);
                Resources().activeScene = resourcePath + "/" + *scene;
                sceneEditor.entityEditor.SetVisible(false);
                Hymn().world.Clear();
                Hymn().world.Load(Hymn().GetPath() + "/" + Resources().activeScene + ".json");
                changeScene = false;
                savePromptWindow.SetVisible(false);
                savePromptWindow.ResetDecision();
                break;

            case 2:
                changeScene = false;
                savePromptWindow.ResetDecision();
                savePromptWindow.SetVisible(false);
                break;

            default:
                break;
            }
        }
    }

    // Create folder.
    if (folderNameWindow.IsVisible())
        folderNameWindow.Show();

    if (sceneEditor.entityPressed || animationClipPressed || animationControllerPressed || skeletonPressed || scriptPressed || texturePressed || modelPressed || soundPressed) {
        sceneEditor.entityEditor.SetVisible(sceneEditor.entityPressed);
        animationClipEditor.SetVisible(animationClipPressed);
        animationControllerEditor.SetVisible(animationControllerPressed);
        skeletonEditor.SetVisible(skeletonPressed);
        scriptEditor.SetVisible(scriptPressed);
        textureEditor.SetVisible(texturePressed);
        modelEditor.SetVisible(modelPressed);
        soundEditor.SetVisible(soundPressed);
    }

    if (sceneEditor.IsVisible()) {
        ImGui::HorizontalSplitter(ImVec2(static_cast<float>(sceneWidth), static_cast<float>(20)), static_cast<int>(size.y - 20), splitterSize, sceneWidth, sceneResize, 20, static_cast<int>(size.x - editorWidth - 20));
        ImGui::SetNextWindowPos(ImVec2(0, 20));
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(sceneWidth), static_cast<float>(size.y - 20)));
        sceneEditor.Show();
    }

    if (sceneEditor.entityEditor.IsVisible() || animationClipEditor.IsVisible() || skeletonEditor.IsVisible() || scriptEditor.IsVisible() || textureEditor.IsVisible() || modelEditor.IsVisible() || soundEditor.IsVisible()) {
        editorWidth = static_cast<int>(size.x - editorWidth);
        ImGui::HorizontalSplitter(ImVec2(static_cast<float>(editorWidth), 20.f), static_cast<int>(size.y - 20), splitterSize, editorWidth, editorResize, static_cast<int>(sceneWidth + 20), static_cast<int>(size.x - 20));
        editorWidth = static_cast<int>(size.x - editorWidth);

        ImGui::SetNextWindowPos(ImVec2(size.x - editorWidth, 20));
        ImGui::SetNextWindowSize(ImVec2(static_cast<float>(editorWidth), static_cast<float>(size.y - 20)));
    }

    if (animationClipEditor.IsVisible())
        animationClipEditor.Show();
    if (animationControllerEditor.IsVisible())
        animationControllerEditor.Show();
    if (skeletonEditor.IsVisible())
        skeletonEditor.Show();
    if (sceneEditor.entityEditor.IsVisible())
        sceneEditor.entityEditor.Show();
    if (scriptEditor.IsVisible())
        scriptEditor.Show();
    if (textureEditor.IsVisible())
        textureEditor.Show();
    if (modelEditor.IsVisible())
        modelEditor.Show();
    if (soundEditor.IsVisible())
        soundEditor.Show();

    ImGui::End();
}

bool ResourceView::HasMadeChanges() const {
    std::string* sceneFilename = new std::string();
    Json::Value sceneJson = sceneEditor.GetSaveFileJson(sceneFilename);

    // Load Json document from file.
    Json::Value reference;
    std::ifstream file(*sceneFilename);

    if (!file.good())
        return true;

    file >> reference;
    file.close();

    std::string hymnJsonString = sceneJson.toStyledString();
    std::string referenceString = reference.toStyledString();

    int response = referenceString.compare(hymnJsonString);
    if (response != 0)
        return true;

    return false;
}

bool ResourceView::IsVisible() const {
    return visible;
}

void ResourceView::SetVisible(bool visible) {
    this->visible = visible;
}

void ResourceView::HideEditors() {
    animationClipEditor.SetVisible(false);
    animationControllerEditor.SetVisible(false);
    skeletonEditor.SetVisible(false);
    sceneEditor.SetVisible(false);
    sceneEditor.entityEditor.SetVisible(false);
    scriptEditor.SetVisible(false);
    modelEditor.SetVisible(false);
    textureEditor.SetVisible(false);
    soundEditor.SetVisible(false);
}

void ResourceView::SaveScene() const {
    sceneEditor.Save();
}

Json::Value ResourceView::GetSceneJson(std::string* filename) const {
    return sceneEditor.GetSaveFileJson(filename);
}

#undef max
void ResourceView::ResetScene() {
    sceneEditor.SetScene("", nullptr);
    sceneEditor.SetVisible(false);
}

SceneEditor& ResourceView::GetScene() {
    return sceneEditor;
}

int ResourceView::GetEditorWidth() const {
    return editorWidth;
}

bool ResourceView::ShowResourceFolder(ResourceList::ResourceFolder& folder, const string& path) {
    string imguiName = folder.name + "##F--" + path;
    bool opened = ImGui::TreeNode(imguiName.c_str());

    if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
        // Add subfolder.
        if (ImGui::Selectable("Add folder")) {
            resourcePath = path;
            parentFolder = &folder;
            folderNameWindow.SetVisible(true);
        }

        // Add scene.
        else if (ImGui::Selectable("Add scene")) {
            ResourceList::Resource resource;
            resource.type = ResourceList::Resource::SCENE;
            resource.scene = new string("Scene #" + std::to_string(Resources().sceneNumber++));
            folder.resources.push_back(resource);
        }

        // Add animation clip.
        if (ImGui::Selectable("Add animation clip")) {
            ResourceList::Resource resource;
            resource.type = ResourceList::Resource::ANIMATION_CLIP;
            resource.animationClip = new Animation::AnimationClip();
            resource.animationClip->path = path + "/";
            resource.animationClip->name = "Animation clip #" + std::to_string(Resources().animationClipNumber++);
            folder.resources.push_back(resource);
        }

        // Add animation controller.
        if (ImGui::Selectable("Add animation controller")) {
            ResourceList::Resource resource;
            resource.type = ResourceList::Resource::ANIMATION_CONTROLLER;
            resource.animationController = new Animation::AnimationController();
            resource.animationController->path = path + "/";
            resource.animationController->name = "Animation controller #" + std::to_string(Resources().animationControllerNumber++);
            folder.resources.push_back(resource);
        }

        // Add skeleton.
        if (ImGui::Selectable("Add skeleton")) {
            ResourceList::Resource resource;
            resource.type = ResourceList::Resource::SKELETON;
            resource.skeleton = new Animation::Skeleton();
            resource.skeleton->path = path + "/";
            resource.skeleton->name = "Skeleton #" + std::to_string(Resources().skeletonNumber++);
            folder.resources.push_back(resource);
        }

        // Add model.
        else if (ImGui::Selectable("Add model")) {
            ResourceList::Resource resource;
            resource.type = ResourceList::Resource::MODEL;
            resource.model = new Geometry::Model();
            resource.model->path = path + "/";
            resource.model->name = "Model #" + std::to_string(Resources().modelNumber++);
            folder.resources.push_back(resource);
        }

        // Add texture.
        else if (ImGui::Selectable("Add texture")) {
            ResourceList::Resource resource;
            resource.type = ResourceList::Resource::TEXTURE;
            string name = path + "/Texture #" + std::to_string(Resources().textureNumber++);
            resource.texture = Managers().resourceManager->CreateTextureAsset(name);
            folder.resources.push_back(resource);
        }

        // Add script.
        else if (ImGui::Selectable("Add script")) {
            ResourceList::Resource resource;
            resource.type = ResourceList::Resource::SCRIPT;
            resource.script = new ScriptFile();
            resource.script->path = path + "/";
            resource.script->name = "Script #" + std::to_string(Hymn().scriptNumber++);
            Hymn().scripts.push_back(resource.script);
            folder.resources.push_back(resource);

            // Save the file with default contents.
            std::string filePath;
            filePath.append(Hymn().GetPath());
            filePath.append("/");
            filePath.append(resource.script->path);
            filePath.append(resource.script->name);
            filePath.append(".as");
            if (!FileSystem::FileExists(filePath.c_str())) {
                std::ofstream file(filePath);
                file << DefaultScriptBody;
                file.close();
            } else
                Log() << "Warning: new script `" << filePath << "` already exists.";
        }

        // Add sound.
        else if (ImGui::Selectable("Add sound")) {
            ResourceList::Resource resource;
            resource.type = ResourceList::Resource::SOUND;
            resource.sound = new Audio::VorbisFile();
            resource.sound->path = path + "/";
            resource.sound->name = "Sound #" + std::to_string(Resources().soundNumber++);
            folder.resources.push_back(resource);
        }

        // Remove Folder.
        else if (folder.subfolders.empty() && folder.resources.empty()) {
            if (ImGui::Selectable("Remove Folder")) {
                ImGui::EndPopup();
                if (opened)
                    ImGui::TreePop();
                return true;
            }
        }
        ImGui::EndPopup();
    }

    if (opened) {
        // Show subfolders.
        for (auto it = folder.subfolders.begin(); it != folder.subfolders.end(); ++it) {
            if (ShowResourceFolder(*it, path + "/" + it->name)) {
                folder.subfolders.erase(it);
                ImGui::TreePop();
                return false;
            }
        }

        // Show resources.
        for (auto it = folder.resources.begin(); it != folder.resources.end(); ++it) {
            if (ShowResource(folder, *it, path)) {
                folder.resources.erase(it);
                ImGui::TreePop();
                return false;
            }
        }

        ImGui::TreePop();
    }
    return false;
}

bool ResourceView::ShowResource(ResourceList::ResourceFolder& folder, ResourceList::Resource& resource, const string& path) {
    // Scene.
    if (resource.type == ResourceList::Resource::SCENE) {
        string imguiName = *resource.scene + "##" + path;
        if (ImGui::Selectable(imguiName.c_str())) {
            // Sets to don't save when opening first scene.
            if (scene == nullptr) {
                changeScene = true;
                resourcePath = path;
                scene = resource.scene;
                savePromptWindow.SetVisible(false);
                savePromptWindow.SetDecision(1);
            } else {
                // Does so that the prompt window won't show if you select active scene.
                if (Resources().activeScene != path + "/" + *resource.scene) {
                    changeScene = true;
                    resourcePath = path;
                    scene = resource.scene;
                    savePromptWindow.SetTitle("Save before you switch scene?");
                }
            }
        }

        // Delete scene.
        if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
            if (ImGui::Selectable("Delete")) {
                if (Resources().activeScene == path + "/" + *resource.scene) {
                    Hymn().world.Clear();
                    scene = nullptr;
                    Resources().activeScene = "";
                    sceneEditor.SetScene("", nullptr);
                }

                ImGui::EndPopup();

                return true;
            }
            ImGui::EndPopup();
        }
    }

    // Animation clip.
    if (resource.type == ResourceList::Resource::ANIMATION_CLIP) {
        string imguiName = resource.animationClip->name + "##" + path;
        if (ImGui::Selectable(imguiName.c_str())) {
            animationClipPressed = true;
            animationClipEditor.SetAnimationClip(resource.animationClip);
        }

        // Delete animation controller.
        if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
            if (ImGui::Selectable("Delete")) {
                if (animationClipEditor.GetAnimationClip() == resource.animationClip)
                    animationClipEditor.SetVisible(false);

                Managers().resourceManager->FreeAnimationClip(resource.animationClip);
                ImGui::EndPopup();

                return true;
            }
            ImGui::EndPopup();
        }
    }

    // Animation controller.
    if (resource.type == ResourceList::Resource::ANIMATION_CONTROLLER) {
        string imguiName = resource.animationController->name + "##" + path;
        if (ImGui::Selectable(imguiName.c_str())) {
            animationControllerPressed = true;
            animationControllerEditor.SetAnimationController(resource.animationController);
        }

        // Delete animation controller.
        if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
            if (ImGui::Selectable("Delete")) {
                if (animationControllerEditor.GetAnimationController() == resource.animationController)
                    animationControllerEditor.SetVisible(false);

                Managers().resourceManager->FreeAnimationController(resource.animationController);
                ImGui::EndPopup();

                return true;
            }
            ImGui::EndPopup();
        }
    }

    // Skeleton.
    if (resource.type == ResourceList::Resource::SKELETON) {
        string imguiName = resource.skeleton->name + "##" + path;
        if (ImGui::Selectable(imguiName.c_str())) {
            skeletonPressed = true;
            skeletonEditor.SetSkeleton(resource.skeleton);
        }

        // Delete skeleton.
        if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
            if (ImGui::Selectable("Delete")) {
                if (skeletonEditor.GetSkeleton() == resource.skeleton)
                    skeletonEditor.SetVisible(false);

                Managers().resourceManager->FreeSkeleton(resource.skeleton);
                ImGui::EndPopup();

                return true;
            }
            ImGui::EndPopup();
        }
    }

    // Model.
    if (resource.type == ResourceList::Resource::MODEL) {
        string imguiName = resource.model->name + "##" + path;
        if (ImGui::Selectable(imguiName.c_str())) {
            modelPressed = true;
            modelEditor.SetModel(&folder, resource.model);
        }

        if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
            if (ImGui::Selectable("Delete")) {
                if (modelEditor.GetModel() == resource.model)
                    modelEditor.SetVisible(false);

                Managers().resourceManager->FreeModel(resource.model);
                ImGui::EndPopup();

                return true;
            }
            ImGui::EndPopup();
        }
    }

    // Textures.
    if (resource.type == ResourceList::Resource::TEXTURE) {
        string imguiName = resource.texture->name + "##" + path;
        if (ImGui::Selectable(imguiName.c_str())) {
            texturePressed = true;
            textureEditor.SetTexture(resource.texture);
        }

        if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
            if (ImGui::Selectable("Delete")) {
                if (Managers().resourceManager->GetTextureAssetInstanceCount(resource.texture) > 1) {
                    Log() << "This texture is in use. Remove all references to the texture first.\n";
                } else {
                    if (textureEditor.GetTexture() == resource.texture)
                        textureEditor.SetVisible(false);

                    // Remove files.
                    remove((Hymn().GetPath() + "/" + path + "/" + resource.texture->name + ".hct").c_str());

                    Managers().resourceManager->FreeTextureAsset(resource.texture);
                    ImGui::EndPopup();
                    return true;
                }
            }
            ImGui::EndPopup();
        }
    }

    // Scripts.
    if (resource.type == ResourceList::Resource::SCRIPT) {
        string imguiName = resource.script->name + "##" + path;

        if (ImGui::Selectable(imguiName.c_str())) {
            scriptPressed = true;
            scriptEditor.SetScript(resource.script);
        }

        if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
            if (ImGui::Selectable("Delete")) {
                if (scriptEditor.GetScript() == resource.script)
                    scriptEditor.SetVisible(false);

                Managers().resourceManager->FreeScriptFile(resource.script);
                for (auto it = Hymn().scripts.begin(); it != Hymn().scripts.end(); ++it) {
                    if (*it == resource.script) {
                        Hymn().scripts.erase(it);
                        break;
                    }
                }
                ImGui::EndPopup();
                return true;
            }
            ImGui::EndPopup();
        }
    }

    // Sounds.
    if (resource.type == ResourceList::Resource::SOUND) {
        string imguiName = resource.sound->name + "##" + path;
        if (ImGui::Selectable(imguiName.c_str())) {
            soundPressed = true;
            soundEditor.SetSound(resource.sound);
        }

        if (ImGui::BeginPopupContextItem(imguiName.c_str())) {
            if (ImGui::Selectable("Delete")) {
                if (soundEditor.GetSound() == resource.sound)
                    soundEditor.SetVisible(false);

                Managers().resourceManager->FreeSound(resource.sound);
                ImGui::EndPopup();
                return true;
            }
            ImGui::EndPopup();
        }
    }

    return false;
}

void ResourceView::FileNameWindowClosed(const std::string& name) {
    if (!name.empty()) {
        ResourceList::ResourceFolder folder;
        folder.name = name;
        parentFolder->subfolders.push_back(folder);

        FileSystem::CreateDirectory((Hymn().GetPath() + "/" + resourcePath + "/" + name).c_str());
    }
}
