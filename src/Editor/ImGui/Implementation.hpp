#pragma once

struct GLFWwindow;

namespace Video {
class Renderer;
}

namespace ImGuiImplementation {
/// Initialize the ImGui implementation.
/**
 * @param window Window to get output for.
 * @param renderer %Renderer to use for rendering the GUI.
 */
void Init(GLFWwindow* window, Video::Renderer* renderer);

/// Shutdown ImGui.
void Shutdown();

/// Start a new frame.
void NewFrame();
} // namespace ImGuiImplementation
