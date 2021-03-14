#pragma once

#include <string>
#include <Editor/Util/GLTFImporter.hpp>

namespace GUI {
/// Used to import GLTF models.
class GLTFEditor {
  public:
    /// Constructor.
    GLTFEditor();

    /// Show the editor.
    void Show();

    /// Set file to import.
    /**
     * @param path Path.
     * @param filename Filename.
     */
    void SetFile(const std::string& path, const std::string& filename);

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
    bool visible = false;
    std::string path;
    std::string filename;

    GLTFImporter gltfImporter;
};
} // namespace GUI
