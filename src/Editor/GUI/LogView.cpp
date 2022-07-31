#include "LogView.hpp"

#include <imgui.h>
#include "../ImGui/Splitter.hpp"
#include <functional>

using namespace GUI;

LogView::LogView() {
    Log::SetupCallback(std::bind(&LogView::MessageCallback, this, std::placeholders::_1, std::placeholders::_2));
}

LogView::~LogView() {
    Log::ResetCallback();
}

void LogView::Show() {
    // Options.
    // Clear log window from text.
    if (ImGui::Button("Clear"))
        textBuffer.clear();

    ImGui::SameLine();

    // Copy log contents to clipboard.
    if (ImGui::Button("Copy to clipboard"))
        ImGui::LogToClipboard();

    ImGui::SameLine();

    // Filter
    textFilter.Draw("Filter");

    ImGui::Separator();

    // Draw log. If filter is active, filter log contents.
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    if (textFilter.IsActive()) {
        const char* bufferBegin = textBuffer.begin();
        const char* line = bufferBegin;
        for (int lineNumber = 0; line != NULL; lineNumber++) {
            const char* lineEnd = (lineNumber < lineOffsets.Size) ? bufferBegin + lineOffsets[lineNumber] : NULL;
            if (textFilter.PassFilter(line, lineEnd))
                ImGui::TextUnformatted(line, lineEnd);
            line = lineEnd && lineEnd[1] ? lineEnd + 1 : NULL;
        }
    } else
        ImGui::TextUnformatted(textBuffer.begin());

    ImGui::EndChild();
}

void LogView::MessageCallback(const Log::Channel channel, const std::string& message) {
    // Add a prefix based on which channel it is.
    std::string prefix;
    switch (channel) {
    case Log::INFO:
        prefix = "[Info]" ;
        break;
    case Log::WARNING:
        prefix = "[Warning]";
        break;
    case Log::ERR:
        prefix = "[Error]";
        break;
    default:
        prefix = "[Default]";
        break;
    }

    const std::string output = prefix + message;

    // Add new lines to text buffer.
    int oldSize = textBuffer.size();

    textBuffer.appendfv(output.c_str(), nullptr);

    for (int newSize = textBuffer.size(); oldSize < newSize; oldSize++) {
        if (textBuffer[oldSize] == '\n')
            lineOffsets.push_back(oldSize);
    }
}
