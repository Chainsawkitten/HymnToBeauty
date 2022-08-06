#pragma once

#include <string>
#include <imgui.h>
#include <Utility/Log.hpp>

namespace GUI {
class LogView {
  public:
    /// Constructor.
    LogView();

    /// Destructor.
    ~LogView();

    /// Shows the log.
    void Show();

  private:
    void MessageCallback(const Log::Channel channel, const std::string& message);

    ImGuiTextBuffer textBuffer;
    ImGuiTextFilter textFilter;
    ImVector<int> lineOffsets;
};
} // namespace GUI
