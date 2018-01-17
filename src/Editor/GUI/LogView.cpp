#include "LogView.hpp"
#include <imgui.h>
#include <Engine/MainWindow.hpp>
#include "../ImGui/Splitter.hpp"
#include <Utility/Log.hpp>
#include <iostream>

using namespace GUI;

LogView::LogView() {
    // Setup streams.
    Log().SetupStream(Log::DEFAULT, &defaultStringstream);
    Log().SetupStream(Log::INFO, &infoStringstream);
    Log().SetupStream(Log::WARNING, &warningStringstream);
    Log().SetupStream(Log::ERR, &errorStringstream);
}

LogView::~LogView() {
    // Reset streams to cout.
    Log().SetupStream(Log::DEFAULT, &std::cout);
    Log().SetupStream(Log::INFO, &std::cout);
    Log().SetupStream(Log::WARNING, &std::cout);
    Log().SetupStream(Log::ERR, &std::cout);
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

void LogView::UpdateLog() {
    //Create log output string.
    std::string output;
    if (!defaultStringstream.str().empty())
        output += "[Default] " + defaultStringstream.str();

    if (!infoStringstream.str().empty())
        output += "[Info] " + infoStringstream.str();

    if (!warningStringstream.str().empty())
        output += "[Warning] " + warningStringstream.str();

    if (!errorStringstream.str().empty())
        output += "[Error] " + errorStringstream.str();

    // Add new lines to text buffer.
    int oldSize = textBuffer.size();

    textBuffer.appendfv(output.c_str(), nullptr);

    for (int newSize = textBuffer.size(); oldSize < newSize; oldSize++)
        if (textBuffer[oldSize] == '\n')
            lineOffsets.push_back(oldSize);

    // Clear streams.
    defaultStringstream.str(std::string());
    infoStringstream.str(std::string());
    warningStringstream.str(std::string());
    errorStringstream.str(std::string());
}
