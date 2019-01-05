#pragma once

struct ImVec2;

namespace ImGui {
/// Handle a horizontal splitter.
/**
 * @param position Position of the splitter.
 * @param height Height of the splitter.
 * @param splitterSize The size of the splitter.
 * @param value The value being manipulated.
 * @param active Bool to hold whether the splitter is active.
 * @param min The minimum allowed value.
 * @param max The maximum allowed value.
 */
void HorizontalSplitter(const ImVec2& position, int height, int splitterSize, int& value, bool& active, int min, int max);

/// Handle a vertical splitter.
/**
 * @param position Position of the splitter.
 * @param width Width of the splitter.
 * @param splitterSize The size of the splitter.
 * @param value The value being manipulated.
 * @param active Bool to hold whether the splitter is active.
 * @param min The minimum allowed value.
 * @param max The maximum allowed value.
 */
void VerticalSplitter(const ImVec2& position, int width, int splitterSize, int& value, bool& active, int min, int max);
} // namespace ImGui
