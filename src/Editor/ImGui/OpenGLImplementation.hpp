#pragma once

struct GLFWwindow;

namespace ImGuiImplementation {
    /// Initialize the ImGui implementation.
    /**
     * @param window Window to get output for.
     */
    void Init(GLFWwindow* window);

    /// Shutdown ImGui.
    void Shutdown();

    /// Start a new frame.
    void NewFrame();
}
