#pragma once

namespace GUI {
/// A window where input is configured.
class InputWindow {
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
    void AddButton();

    bool visible = false;
};
} // namespace GUI
