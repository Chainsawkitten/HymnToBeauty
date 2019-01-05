#pragma once

#include <functional>
#include <vector>
#include <string>

namespace GUI {
/// A window that asks the user if they want to save before quitting.
class SavePromptWindow {
  public:
    /// Show the window and let the user decide if they want to save.
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

    /// Sets decision to true or false.
    /**
     * @param option If the decision should be 0: Save, 1: Don't save, *2: cancel.
     */
    void SetDecision(int option);

    /// Fetches what the player picked.
    /**
     * @return 0: Save, 1: Don't save, *2: cancel.*
     */
    int GetDecision() const;

    /// Resets the decision to -1.
    void ResetDecision();

    /// Set the title of the window.
    /**
     * @param newTitle The new title of the window.
     */
    void SetTitle(const std::string& newTitle);

  private:
    std::string title;
    bool visible = false;
    int decision = -1;
};
} // namespace GUI
