#pragma once

struct GLFWwindow;

namespace ImGuiImplementation {
    bool Init(GLFWwindow* window, bool install_callbacks);
    void Shutdown();
    void NewFrame();
    
    // Use if you want to reset your rendering device without losing ImGui state.
    void InvalidateDeviceObjects();
    bool CreateDeviceObjects();
}
