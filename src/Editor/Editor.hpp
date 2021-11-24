#pragma once

#include "GUI/SelectHymnWindow.hpp"
#include "GUI/InputWindow.hpp"
#include "GUI/ResourceView.hpp"
#include "GUI/SettingsWindow.hpp"
#include "GUI/SavePromptWindow.hpp"
#include "GUI/ProfilingWindow.hpp"
#include "GUI/LogView.hpp"
#include <Engine/Entity/World.hpp>
#include <Engine/Util/MousePicking.hpp>
#include <Engine/Util/RayIntersection.hpp>
#include <string>
#include <json/json.h>
#include <ImGuizmo.h>

struct GLFWcursor;
namespace Video {
class LowLevelRenderer;
}

/// Handles the main editor.
class Editor {
  public:
    /// Create new editor.
    /**
     * @param lowLevelRenderer The low-level renderer to use.
     */
    explicit Editor(Video::LowLevelRenderer* lowLevelRenderer);

    /// Destructor.
    ~Editor();

    /// Show the editor.
    /**
     * @param deltaTime Time since last frame (in seconds).
     */
    void Show(float deltaTime);

    /// Show the editor.
    /**
     * @return Check if you have made any changes to the scene.
     */
    bool HasMadeChanges() const;

    /// Save the hymn being edited.
    void Save() const;

    /// Loads the save editor state.
    void LoadSceneState();

    /// Is the editor ready to be closed, have we saved everything?
    /**
     * @return Are we ready to close the editor?
     */
    bool ReadyToClose() const;

    /// Is the editor preparing for closing down?
    /**
     * @return Are we closing the editor?
     */
    bool isClosing() const;

    /// Close the editor.
    void Close();

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

    /// Get the editor camera.
    /**
     * @return Camera through which to render.
     */
    Entity* GetCamera() const;

    // Whether the vertices for the paint mode are loaded or not.
    bool vertsLoaded = false;

    // Whether the paint mode is active or not.
    bool paintModeActive = false;

  private:
    void ShowMainMenuBar(bool& play);
    void ShowGridSettings();
    void CreateGrid(int size);
    void ControlEditorCamera(float deltaTime);
    void Picking();
    void Focus();
    void PaintBrush(Entity* entity);
    void WidgetGizmo(Entity* entity);

    void Play();
    void NewHymn();
    void NewHymnClosed(const std::string& hymn);
    void OpenHymn();
    void OpenHymnClosed(const std::string& hymn);
    void LoadActiveScene();

    struct GridSettings {
        int gridSize;
        int lineWidth;
        bool gridSnap;
        int snapOption;
    } gridSettings;

    bool visible = true;
    GUI::SelectHymnWindow selectHymnWindow;
    GUI::InputWindow inputWindow;
    GUI::ResourceView resourceView;
    GUI::LogView logView;
    GUI::SettingsWindow settingsWindow;
    GUI::SavePromptWindow savePromptWindow;
    GUI::ProfilingWindow profilingWindow;

    bool close;
    bool savePromptAnswered;
    bool showGridSettings;
    Entity* parentEntity = nullptr;
    Json::Value sceneState;

    World cameraWorld;
    Entity* cameraEntity;
    Entity* currentEntity;
    Entity* selectedEntity;
    MousePicking mousePicker;
    RayIntersection rayIntersector;
    int nrOfVertices = 0;
    int nrOfIndices = 0;
    glm::vec3 normal;
    float lastIntersect = INFINITY;
    double lastX = 0.0;
    double lastY = 0.0;

    // Paint brush variables.
    float paintTimer = 0.0f;
    float paintSpawnRate[1] = {0.5f};
    float paintObjScale[1] = {1.0f};
    int paintScaleRandomness[1] = {1};
    bool toolMenuPressed = false;
    bool spreadRand = false;
    std::string paintScene;

    GLFWcursor* cursors[5];

    ImGuizmo::MODE imguizmoMode = ImGuizmo::MODE::WORLD;
};
