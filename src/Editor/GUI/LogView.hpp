#pragma once
#include <sstream>
#include <imgui.h>

namespace GUI {
class LogView {
  public:
    /// Constructor.
    LogView();

    /// Destructor.
    ~LogView();

    /// Shows the log.
    void Show();

    // Updates the log.
    void UpdateLog();

  private:
    std::stringstream defaultStringstream;
    std::stringstream infoStringstream;
    std::stringstream warningStringstream;
    std::stringstream errorStringstream;

    ImGuiTextBuffer textBuffer;
    ImGuiTextFilter textFilter;
    ImVector<int> lineOffsets;
};
} // namespace GUI
