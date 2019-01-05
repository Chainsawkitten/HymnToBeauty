#pragma once

namespace GUI {
/// A window where filters are configured.
class FiltersWindow {
  public:
    /// Show the window
    void Show();

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
};
} // namespace GUI
