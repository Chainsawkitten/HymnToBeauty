#pragma once

#include "../FileSelector.hpp"
#include "../../Resources.hpp"

namespace Animation {
class Skeleton;
}

namespace GUI {
/// Editor for skeleton.
class SkeletonEditor {
  public:
    /// Constructor.
    SkeletonEditor();

    /// Show the editor.
    void Show();

    /// Returns the active skeleton.
    /**
     * @return Active skeleton.
     */
    Animation::Skeleton* GetSkeleton();

    /// Set the skeleton to edit.
    /**
     * @param skeleton Skeleton to edit.
     */
    void SetSkeleton(Animation::Skeleton* skeleton);

    /// Get whether the window is visible.
    /**
     * @return True if visible.
     */
    bool IsVisible() const;

    /// Set whether the window should be visible.
    /**
     * @param visible Whether the window should be visible.
     */
    void SetVisible(bool visible);

  private:
    void FileSelected(const std::string& file);

    ResourceList::ResourceFolder* folder = nullptr;
    Animation::Skeleton* skeleton = nullptr;
    bool visible = false;

    FileSelector fileSelector;

    char name[128];
};
} // namespace GUI
