#pragma once

#include "Editors/GLTFEditor.hpp"
#include "Editors/SceneEditor.hpp"
#include "Editors/SoundEditor.hpp"
#include "Editors/ScriptEditor.hpp"
#include "Editors/TextureEditor.hpp"
#include "SavePromptWindow.hpp"
#include "FolderNameWindow.hpp"
#include "../Resources.hpp"

namespace Utility {
class Window;
}
namespace Video {
class LowLevelRenderer;
}

namespace GUI {
/// Displays all the hymn's resources.
class ResourceView {
  public:
    /// Create new resource view.
    /**
     * @param window The window the display the resource view in.
     * @param lowLevelRenderer The low-level renderer to use.
     */
    ResourceView(Utility::Window* window, Video::LowLevelRenderer* lowLevelRenderer);

    /// Show the resource list.
    void Show();

    /// Checks if any changes has been made to the current scene.
    /**
     * @return Has any changes been made.
     */
    bool HasMadeChanges() const;

    /// Get whether the resource list is visible.
    /**
     * @return Whether the resource list is visible.
     */
    bool IsVisible() const;

    /// Set whether the resource list should be visible.
    /**
     * @param visible Whether the resource list should be visible.
     */
    void SetVisible(bool visible);

    /// Hide all editors.
    /**
     * Needs to be called before playing the game or old editors with stale pointers could be shown when returning to the editor.
     */
    void HideEditors();

    /// Save the currently active scene.
    void SaveScene() const;

    /// Get a json representing the scene.
    /**
     * @param filename The json file representing the scene.
     */
    Json::Value GetSceneJson(std::string* filename) const;

    /// Reset which scene is open.
    void ResetScene();

    /// Return Scene Editor
    /**
     * @return The scene editor
     */
    SceneEditor& GetScene();

    /// Get the width of the right-side editor (entity, script, etc.).
    /**
     * @return The width of the editor.
     */
    int GetEditorWidth() const;

  private:
    bool ShowResourceFolder(ResourceList::ResourceFolder& folder, const std::string& path);
    bool ShowResource(ResourceList::ResourceFolder& folder, ResourceList::Resource& resource, const std::string& path);
    void FileNameWindowClosed(const std::string& name);

    Utility::Window* window;
    Video::LowLevelRenderer* lowLevelRenderer;

    bool visible = false;

    ScriptEditor scriptEditor;
    SceneEditor sceneEditor;
    GLTFEditor gltfEditor;
    TextureEditor textureEditor;
    SoundEditor soundEditor;

    SavePromptWindow savePromptWindow;
    FolderNameWindow folderNameWindow;

    bool changeScene = false;
    std::string resourcePath = "";
    std::string* scene = nullptr;
    ResourceList::ResourceFolder* parentFolder;

    static const int splitterSize = 2;
    int resourceHeight = 250;
    bool resourceResize = false;

    int sceneWidth = 250;
    bool sceneResize = false;

    int editorWidth = 250;
    bool editorResize = false;

    bool scriptPressed;
    bool texturePressed;
    bool gltfPressed;
    bool soundPressed;
};
} // namespace GUI
