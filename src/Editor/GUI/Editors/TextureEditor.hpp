#pragma once

#include <string>
#include "../FileSelector.hpp"

class TextureAsset;

namespace GUI {
/// Used to edit a texture.
class TextureEditor {
  public:
    /// Constructor.
    TextureEditor();

    /// Show the editor.
    void Show();

    /// Get the texture being edited.
    /**
     * @return The texture being edited.
     */
    const TextureAsset* GetTexture() const;

    /// Set the texture to edit.
    /**
     * @param texture %Texture to edit.
     */
    void SetTexture(TextureAsset* texture);

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
    void FileSelected(const std::string& file);

    TextureAsset* texture = nullptr;
    bool visible = false;

    FileSelector fileSelector;

    char name[128];

    std::string path;
    int compressionType = 0;
};
} // namespace GUI
