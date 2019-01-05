#pragma once

#include <functional>
#include <vector>

namespace GUI {
/// A window where a file can be selected.
class FileSelector {
  public:
    /// Create new window.
    FileSelector();

    /// Show the file selector.
    void Show();

    /// Set function to call when a file has been selected.
    /**
     * @param callback Function to call.
     */
    void SetFileSelectedCallback(const std::function<void(const std::string&)>& callback);

    /// Set the initial path.
    /**
     * @param path Initial path.
     */
    void SetInitialPath(const char* path);

    /// Set extensions to select files of.
    /**
     * @param extensions Extensions of which files to show.
     */
    void SetExtensions(const std::vector<std::string>& extensions);

    /// Add extension to select files of.
    /**
     * @param extension An extension of which files to show.
     */
    void AddExtensions(const std::string& extension);

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
    void OpenParentDirectory();
    void OpenDirectory(const std::string& name);
    void SelectFile(const std::string& name);
    void ScanDirectory();

    bool visible = false;

    // Interaction
    std::string path;
    std::vector<std::string> extensions;
    bool pathChanged = true;

    std::vector<std::string> directories;
    std::vector<std::string> files;

    bool hasFileSelectedCallback = false;
    std::function<void(const std::string&)> fileSelectedCallback;
};
} // namespace GUI
