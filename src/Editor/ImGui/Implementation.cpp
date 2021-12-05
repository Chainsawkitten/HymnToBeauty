#include "Implementation.hpp"

#include <cstring>
#include <imgui.h>

#include <Engine/Util/Input.hpp>
#include <Utility/Profiling/Profiling.hpp>
#include <Video/Renderer.hpp>
#include <Video/LowLevelRenderer/Interface/LowLevelRenderer.hpp>
#include <Video/LowLevelRenderer/Interface/CommandBuffer.hpp>
#include <Video/LowLevelRenderer/Interface/ShaderProgram.hpp>
#include <Video/LowLevelRenderer/Interface/GraphicsPipeline.hpp>

#include "ImGUI.vert.hpp"
#include "ImGUI.frag.hpp"

#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include <GLFW/glfw3native.h>
#endif

namespace ImGuiImplementation {
// Forward declarations.
static const char* GetClipboardText(void* userData);
static void SetClipboardText(void* userData, const char* text);
static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void CharCallback(GLFWwindow* window, unsigned int c);
static void CreateFontsTexture();
static void CreateDeviceObjects();
static void InvalidateDeviceObjects();

// Data
static GLFWwindow* g_Window = nullptr;
static Video::Renderer* g_Renderer = nullptr;
static Video::LowLevelRenderer* g_LowLevelRenderer = nullptr;
static double g_Time = 0.0f;
static bool g_MousePressed[3] = {false, false, false};
static float g_MouseWheel = 0.0f;
static Video::Texture* g_FontTexture = nullptr;
static Video::Shader* g_VertexShader = nullptr;
static Video::Shader* g_FragmentShader = nullptr;
static Video::ShaderProgram* g_ShaderProgram = nullptr;
static Video::GraphicsPipeline* g_GraphicsPipeline = nullptr;
static unsigned int g_BufferMemoryAlignment = 256;
static const unsigned int g_VertexBufferSize = 40000;
static ImDrawVert* g_VertexData = nullptr;
static const unsigned int g_IndexBufferSize = 100000;
static ImDrawIdx* g_IndexData = nullptr;
static Video::VertexDescription* g_VertexDescription = nullptr;

void Init(GLFWwindow* window, Video::Renderer* renderer) {
    g_Window = window;
    g_Renderer = renderer;
    g_LowLevelRenderer = renderer->GetLowLevelRenderer();

    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    // Set callbacks.
    io.SetClipboardTextFn = SetClipboardText;
    io.GetClipboardTextFn = GetClipboardText;
    io.ClipboardUserData = g_Window;
#ifdef _WIN32
    io.ImeWindowHandle = glfwGetWin32Window(g_Window);
#endif

    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetCharCallback(window, CharCallback);
}

void Shutdown() {
    InvalidateDeviceObjects();
    ImGui::DestroyContext();
}

void NewFrame() {
    if (g_FontTexture == nullptr)
        CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    glfwGetWindowSize(g_Window, &w, &h);
    glfwGetFramebufferSize(g_Window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

    // Setup time step
    double current_time = glfwGetTime();
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    if (glfwGetWindowAttrib(g_Window, GLFW_FOCUSED)) {
        double mouse_x, mouse_y;
        glfwGetCursorPos(g_Window, &mouse_x, &mouse_y);
        io.MousePos = ImVec2((float)mouse_x, (float)mouse_y); // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
    } else {
        io.MousePos = ImVec2(-1, -1);
    }

    for (int i = 0; i < 3; i++) {
        io.MouseDown[i] = g_MousePressed[i] || glfwGetMouseButton(g_Window, i) != 0; // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        g_MousePressed[i] = false;
    }

    io.MouseWheel = g_MouseWheel;
    g_MouseWheel = 0.0f;

    // Hide OS mouse cursor if ImGui is drawing it
    glfwSetInputMode(g_Window, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    // Start the frame
    ImGui::NewFrame();
}

void Render() {
    PROFILE("Render ImGui");

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    ImGuiIO& io = ImGui::GetIO();
    int fbWidth = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fbHeight = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fbWidth == 0 || fbHeight == 0)
        return;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    assert(draw_data->TotalVtxCount < g_VertexBufferSize);
    assert(draw_data->TotalIdxCount < g_IndexBufferSize);

    // Upload vertex and index buffer data.
    ImDrawVert* vertexMemory = g_VertexData;
    ImDrawIdx* indexMemory = g_IndexData;

    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmdList = draw_data->CmdLists[n];
        memcpy(vertexMemory, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(indexMemory, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
        vertexMemory += cmdList->VtxBuffer.Size;
        indexMemory += cmdList->IdxBuffer.Size;
    }

    Video::Buffer* vertexBuffer = g_LowLevelRenderer->CreateTemporaryBuffer(Video::Buffer::BufferUsage::VERTEX_BUFFER, g_VertexBufferSize * sizeof(ImDrawVert), g_VertexData);
    Video::Buffer* indexBuffer = g_LowLevelRenderer->CreateTemporaryBuffer(Video::Buffer::BufferUsage::INDEX_BUFFER, g_IndexBufferSize * sizeof(ImDrawIdx), g_IndexData);
    Video::GeometryBinding* geometryBinding = g_LowLevelRenderer->CreateGeometryBinding(g_VertexDescription, vertexBuffer, Video::GeometryBinding::IndexType::SHORT, indexBuffer);

    // Record rendering commands.
    Video::CommandBuffer* commandBuffer = g_Renderer->GetCommandBuffer();

    commandBuffer->BindGraphicsPipeline(g_GraphicsPipeline);

    // Setup viewport, orthographic projection matrix
    commandBuffer->SetViewport(glm::uvec2(0, 0), glm::uvec2(fbWidth, fbHeight));
    const glm::mat4 orthoProjection = {
        {2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
        {-1.0f, 1.0f, 0.0f, 1.0f},
    };
    commandBuffer->PushConstants(&orthoProjection);
    commandBuffer->BindGeometry(geometryBinding);

    // Draw all the commands in the draw list.
    unsigned int baseVertex = 0;
    unsigned int firstIndex = 0;
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList* cmdList = draw_data->CmdLists[n];

        for (int i = 0; i < cmdList->CmdBuffer.Size; i++) {
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmdList, pcmd);
            } else {
                commandBuffer->BindMaterial({static_cast<Video::Texture*>(pcmd->TextureId)});
                commandBuffer->SetScissor(glm::uvec2((int)pcmd->ClipRect.x, (int)(pcmd->ClipRect.y)), glm::uvec2((int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y)));
                commandBuffer->DrawIndexed(pcmd->ElemCount, firstIndex, baseVertex);
            }
            firstIndex += pcmd->ElemCount;
        }
        baseVertex += cmdList->VtxBuffer.Size;
    }

    delete geometryBinding;
}

static const char* GetClipboardText(void* userData) {
    return glfwGetClipboardString(static_cast<GLFWwindow*>(userData));
}

static void SetClipboardText(void* userData, const char* text) {
    glfwSetClipboardString(static_cast<GLFWwindow*>(userData), text);
}

void MouseButtonCallback(GLFWwindow*, int button, int action, int /*mods*/) {
    if (action == GLFW_PRESS && button >= 0 && button < 3)
        g_MousePressed[button] = true;
}

void ScrollCallback(GLFWwindow*, double /*xoffset*/, double yoffset) {
    g_MouseWheel += (float)yoffset; // Use fractional mouse wheel, 1.0 unit 5 lines.
    Input()->ScrollCallback(yoffset);
}

void KeyCallback(GLFWwindow*, int key, int, int action, int mods) {
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    (void)mods; // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
    io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
}

void CharCallback(GLFWwindow*, unsigned int c) {
    ImGuiIO& io = ImGui::GetIO();
    if (c > 0 && c < 0x10000)
        io.AddInputCharacter((unsigned short)c);
}

void CreateFontsTexture() {
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders.
    // If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // Will generate mipmaps, which isn't really necessary for the font texture.
    g_FontTexture = g_LowLevelRenderer->CreateTexture(glm::uvec2(width, height), Video::Texture::Type::COLOR, Video::Texture::Format::R8G8B8A8, 4, pixels);

    // Store our identifier
    io.Fonts->TexID = g_FontTexture;
}

void CreateDeviceObjects() {
    g_VertexShader = g_LowLevelRenderer->CreateShader(IMGUI_VERT, Video::Shader::Type::VERTEX_SHADER);
    g_FragmentShader = g_LowLevelRenderer->CreateShader(IMGUI_FRAG, Video::Shader::Type::FRAGMENT_SHADER);
    g_ShaderProgram = g_LowLevelRenderer->CreateShaderProgram({g_VertexShader, g_FragmentShader});

    Video::VertexDescription::Attribute attributes[3];

    attributes[0].size = 2;
    attributes[0].type = Video::VertexDescription::AttributeType::FLOAT;
    attributes[0].normalized = false;

    attributes[1].size = 2;
    attributes[1].type = Video::VertexDescription::AttributeType::FLOAT;
    attributes[1].normalized = false;

    attributes[2].size = 4;
    attributes[2].type = Video::VertexDescription::AttributeType::UNSIGNED_BYTE;
    attributes[2].normalized = true;

    g_VertexDescription = g_LowLevelRenderer->CreateVertexDescription(3, attributes, true);

    Video::GraphicsPipeline::Configuration configuration = {};
    configuration.primitiveType = Video::PrimitiveType::TRIANGLE;
    configuration.polygonMode = Video::PolygonMode::FILL;
    configuration.cullFace = Video::CullFace::NONE;
    configuration.blendMode = Video::BlendMode::ALPHA_ONE_MINUS_SRC_ALPHA;
    configuration.depthMode = Video::DepthMode::NONE;

    g_GraphicsPipeline = g_LowLevelRenderer->CreateGraphicsPipeline(g_ShaderProgram, configuration, g_VertexDescription);

    g_VertexData = new ImDrawVert[g_VertexBufferSize];
    g_IndexData = new ImDrawIdx[g_IndexBufferSize];

    CreateFontsTexture();
}

void InvalidateDeviceObjects() {
    delete g_VertexDescription;
    g_VertexDescription = nullptr;

    delete[] g_VertexData;
    delete[] g_IndexData;

    delete g_GraphicsPipeline;
    g_GraphicsPipeline = nullptr;

    delete g_ShaderProgram;
    g_ShaderProgram = nullptr;

    delete g_VertexShader;
    g_VertexShader = nullptr;

    delete g_FragmentShader;
    g_FragmentShader = nullptr;

    if (g_FontTexture != nullptr) {
        delete g_FontTexture;
        ImGui::GetIO().Fonts->TexID = 0;
        g_FontTexture = nullptr;
    }
}
} // namespace ImGuiImplementation
