#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Geometry {
class Model;
}

namespace GUI {
/// Used to edit models.
class AssetEditor {
  public:
    /// Constructor.
    AssetEditor();

    /// Show the editor.
    void Show();

    /// Get the model being edited.
    /**
     * @return The model being edited.
     */
    const Geometry::Model* GetModel() const;

    /// Set the model to edit.
    /**
     * @param model Model to edit.
     */
    void SetModel(Geometry::Model* model);

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
    Geometry::Model* model = nullptr;
    bool visible = false;

    char name[128];

    std::string destination;
};
} // namespace GUI
