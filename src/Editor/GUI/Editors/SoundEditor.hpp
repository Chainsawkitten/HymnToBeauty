#pragma once

#include <string>

namespace GUI {
/// Used to edit a sound.
class SoundEditor {
  public:
    /// Constructor.
    SoundEditor();

    /// Show the editor.
    void Show();

    /// Set sound file.
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
};
} // namespace GUI
