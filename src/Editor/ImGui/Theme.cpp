#include "Theme.hpp"

#include <Engine/Util/FileSystem.hpp>
#include <Engine/Util/Json.hpp>
#include <json/json.h>
#include <fstream>
#include <imgui.h>

namespace ImGui {
    void SaveTheme(const char* name) {
        // Make sure Themes directory exists.
        FileSystem::CreateDirectory((FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Themes").c_str());
        
        // Create JSON representation of theme.
        Json::Value root;
        
        ImGuiStyle& style = ImGui::GetStyle();
        
        root["WindowPadding"] = Json::SaveImVec2(style.WindowPadding);
        root["WindowRounding"] = style.WindowRounding;
        root["FramePadding"] = Json::SaveImVec2(style.FramePadding);
        root["FrameRounding"] = style.FrameRounding;
        root["ItemSpacing"] = Json::SaveImVec2(style.ItemSpacing);
        root["ItemInnerSpacing"] = Json::SaveImVec2(style.ItemInnerSpacing);
        root["IndentSpacing"] = style.IndentSpacing;
        root["ScrollbarSize"] = style.ScrollbarSize;
        root["ScrollbarRounding"] = style.ScrollbarRounding;
        root["GrabMinSize"] = style.GrabMinSize;
        root["GrabRounding"] = style.GrabRounding;
        
        root["Color: Text"] = Json::SaveImVec4(style.Colors[ImGuiCol_Text]);
        root["Color: TextDisabled"] = Json::SaveImVec4(style.Colors[ImGuiCol_TextDisabled]);
        root["Color: WindowBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_WindowBg]);
        root["Color: ChildWindowBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_ChildWindowBg]);
        root["Color: PopupBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_PopupBg]);
        root["Color: Border"] = Json::SaveImVec4(style.Colors[ImGuiCol_Border]);
        root["Color: BorderShadow"] = Json::SaveImVec4(style.Colors[ImGuiCol_BorderShadow]);
        root["Color: FrameBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_FrameBg]);
        root["Color: FrameBgHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_FrameBgHovered]);
        root["Color: FrameBgActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_FrameBgActive]);
        root["Color: TitleBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_TitleBg]);
        root["Color: TitleBgCollapsed"] = Json::SaveImVec4(style.Colors[ImGuiCol_TitleBgCollapsed]);
        root["Color: TitleBgActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_TitleBgActive]);
        root["Color: MenuBarBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_MenuBarBg]);
        root["Color: ScrollbarBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_ScrollbarBg]);
        root["Color: ScrollbarGrab"] = Json::SaveImVec4(style.Colors[ImGuiCol_ScrollbarGrab]);
        root["Color: ScrollbarGrabHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_ScrollbarGrabHovered]);
        root["Color: ScrollbarGrabActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_ScrollbarGrabActive]);
        root["Color: ComboBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_ComboBg]);
        root["Color: CheckMark"] = Json::SaveImVec4(style.Colors[ImGuiCol_CheckMark]);
        root["Color: SliderGrab"] = Json::SaveImVec4(style.Colors[ImGuiCol_SliderGrab]);
        root["Color: SliderGrabActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_SliderGrabActive]);
        root["Color: Button"] = Json::SaveImVec4(style.Colors[ImGuiCol_Button]);
        root["Color: ButtonHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_ButtonHovered]);
        root["Color: ButtonActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_ButtonActive]);
        root["Color: Header"] = Json::SaveImVec4(style.Colors[ImGuiCol_Header]);
        root["Color: HeaderHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_HeaderHovered]);
        root["Color: HeaderActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_HeaderActive]);
        root["Color: Column"] = Json::SaveImVec4(style.Colors[ImGuiCol_Column]);
        root["Color: ColumnHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_ColumnHovered]);
        root["Color: ColumnActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_ColumnActive]);
        root["Color: ResizeGrip"] = Json::SaveImVec4(style.Colors[ImGuiCol_ResizeGrip]);
        root["Color: ResizeGripHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_ResizeGripHovered]);
        root["Color: ResizeGripActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_ResizeGripActive]);
        root["Color: CloseButton"] = Json::SaveImVec4(style.Colors[ImGuiCol_CloseButton]);
        root["Color: CloseButtonHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_CloseButtonHovered]);
        root["Color: CloseButtonActive"] = Json::SaveImVec4(style.Colors[ImGuiCol_CloseButtonActive]);
        root["Color: PlotLines"] = Json::SaveImVec4(style.Colors[ImGuiCol_PlotLines]);
        root["Color: PlotLinesHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_PlotLinesHovered]);
        root["Color: PlotHistogram"] = Json::SaveImVec4(style.Colors[ImGuiCol_PlotHistogram]);
        root["Color: PlotHistogramHovered"] = Json::SaveImVec4(style.Colors[ImGuiCol_PlotHistogramHovered]);
        root["Color: TextSelectedBg"] = Json::SaveImVec4(style.Colors[ImGuiCol_TextSelectedBg]);
        root["Color: ModalWindowDarkening"] = Json::SaveImVec4(style.Colors[ImGuiCol_ModalWindowDarkening]);
        
        // Save to file.
        std::ofstream file(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Themes" + FileSystem::DELIMITER + name + ".json");
        file << root;
        file.close();
    }
    
    void LoadTheme(const char* name) {
        // Load Json document from file.
        Json::Value root;
        std::ifstream file(FileSystem::DataPath("Hymn to Beauty") + FileSystem::DELIMITER + "Themes" + FileSystem::DELIMITER + name + ".json");
        file >> root;
        file.close();
        
        // Get theme from JSON representation.
        ImGuiStyle& style = ImGui::GetStyle();
        
        style.WindowPadding = Json::LoadImVec2(root["WindowPadding"]);
        style.WindowRounding = root.get("WindowRounding", 0.0f).asFloat();
        style.FramePadding = Json::LoadImVec2(root["FramePadding"]);
        style.FrameRounding = root.get("FrameRounding", 0.0f).asFloat();
        style.ItemSpacing = Json::LoadImVec2(root["ItemSpacing"]);
        style.ItemInnerSpacing = Json::LoadImVec2(root["ItemInnerSpacing"]);
        style.IndentSpacing = root.get("IndentSpacing", 0.0f).asFloat();
        style.ScrollbarSize = root.get("ScrollbarSize", 0.0f).asFloat();
        style.ScrollbarRounding = root.get("ScrollbarRounding", 0.0f).asFloat();
        style.GrabMinSize = root.get("GrabMinSize", 0.0f).asFloat();
        style.GrabRounding = root.get("GrabRounding", 0.0f).asFloat();
        
        style.Colors[ImGuiCol_Text] = Json::LoadImVec4(root["Color: Text"]);
        style.Colors[ImGuiCol_TextDisabled] = Json::LoadImVec4(root["Color: TextDisabled"]);
        style.Colors[ImGuiCol_WindowBg] = Json::LoadImVec4(root["Color: WindowBg"]);
        style.Colors[ImGuiCol_ChildWindowBg] = Json::LoadImVec4(root["Color: ChildWindowBg"]);
        style.Colors[ImGuiCol_PopupBg] = Json::LoadImVec4(root["Color: PopupBg"]);
        style.Colors[ImGuiCol_Border] = Json::LoadImVec4(root["Color: Border"]);
        style.Colors[ImGuiCol_BorderShadow] = Json::LoadImVec4(root["Color: BorderShadow"]);
        style.Colors[ImGuiCol_FrameBg] = Json::LoadImVec4(root["Color: FrameBg"]);
        style.Colors[ImGuiCol_FrameBgHovered] = Json::LoadImVec4(root["Color: FrameBgHovered"]);
        style.Colors[ImGuiCol_FrameBgActive] = Json::LoadImVec4(root["Color: FrameBgActive"]);
        style.Colors[ImGuiCol_TitleBg] = Json::LoadImVec4(root["Color: TitleBg"]);
        style.Colors[ImGuiCol_TitleBgCollapsed] = Json::LoadImVec4(root["Color: TitleBgCollapsed"]);
        style.Colors[ImGuiCol_TitleBgActive] = Json::LoadImVec4(root["Color: TitleBgActive"]);
        style.Colors[ImGuiCol_MenuBarBg] = Json::LoadImVec4(root["Color: MenuBarBg"]);
        style.Colors[ImGuiCol_ScrollbarBg] = Json::LoadImVec4(root["Color: ScrollbarBg"]);
        style.Colors[ImGuiCol_ScrollbarGrab] = Json::LoadImVec4(root["Color: ScrollbarGrab"]);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = Json::LoadImVec4(root["Color: ScrollbarGrabHovered"]);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = Json::LoadImVec4(root["Color: ScrollbarGrabActive"]);
        style.Colors[ImGuiCol_ComboBg] = Json::LoadImVec4(root["Color: ComboBg"]);
        style.Colors[ImGuiCol_CheckMark] = Json::LoadImVec4(root["Color: CheckMark"]);
        style.Colors[ImGuiCol_SliderGrab] = Json::LoadImVec4(root["Color: SliderGrab"]);
        style.Colors[ImGuiCol_SliderGrabActive] = Json::LoadImVec4(root["Color: SliderGrabActive"]);
        style.Colors[ImGuiCol_Button] = Json::LoadImVec4(root["Color: Button"]);
        style.Colors[ImGuiCol_ButtonHovered] = Json::LoadImVec4(root["Color: ButtonHovered"]);
        style.Colors[ImGuiCol_ButtonActive] = Json::LoadImVec4(root["Color: ButtonActive"]);
        style.Colors[ImGuiCol_Header] = Json::LoadImVec4(root["Color: Header"]);
        style.Colors[ImGuiCol_HeaderHovered] = Json::LoadImVec4(root["Color: HeaderHovered"]);
        style.Colors[ImGuiCol_HeaderActive] = Json::LoadImVec4(root["Color: HeaderActive"]);
        style.Colors[ImGuiCol_Column] = Json::LoadImVec4(root["Color: Column"]);
        style.Colors[ImGuiCol_ColumnHovered] = Json::LoadImVec4(root["Color: ColumnHovered"]);
        style.Colors[ImGuiCol_ColumnActive] = Json::LoadImVec4(root["Color: ColumnActive"]);
        style.Colors[ImGuiCol_ResizeGrip] = Json::LoadImVec4(root["Color: ResizeGrip"]);
        style.Colors[ImGuiCol_ResizeGripHovered] = Json::LoadImVec4(root["Color: ResizeGripHovered"]);
        style.Colors[ImGuiCol_ResizeGripActive] = Json::LoadImVec4(root["Color: ResizeGripActive"]);
        style.Colors[ImGuiCol_CloseButton] = Json::LoadImVec4(root["Color: CloseButton"]);
        style.Colors[ImGuiCol_CloseButtonHovered] = Json::LoadImVec4(root["Color: CloseButtonHovered"]);
        style.Colors[ImGuiCol_CloseButtonActive] = Json::LoadImVec4(root["Color: CloseButtonActive"]);
        style.Colors[ImGuiCol_PlotLines] = Json::LoadImVec4(root["Color: PlotLines"]);
        style.Colors[ImGuiCol_PlotLinesHovered] = Json::LoadImVec4(root["Color: PlotLinesHovered"]);
        style.Colors[ImGuiCol_PlotHistogram] = Json::LoadImVec4(root["Color: PlotHistogram"]);
        style.Colors[ImGuiCol_PlotHistogramHovered] = Json::LoadImVec4(root["Color: PlotHistogramHovered"]);
        style.Colors[ImGuiCol_TextSelectedBg] = Json::LoadImVec4(root["Color: TextSelectedBg"]);
        style.Colors[ImGuiCol_ModalWindowDarkening] = Json::LoadImVec4(root["Color: ModalWindowDarkening"]);
    }
    
    void LoadDefaultTheme() {
        ImGuiStyle& style = ImGui::GetStyle();
        
        style.WindowPadding = ImVec2(8.0f, 8.0f);
        style.WindowRounding = 9.0f;
        style.FramePadding = ImVec2(4.0f, 3.0f);
        style.FrameRounding = 0.0f;
        style.ItemSpacing = ImVec2(8.0f, 4.0f);
        style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
        style.IndentSpacing = 21.0f;
        style.ScrollbarSize = 16.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 10.0f;
        style.GrabRounding = 0.0f;
        
        style.Colors[ImGuiCol_Text] = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
        style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.1f, 0.9f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.7f, 0.7f, 0.7f, 0.65f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.3f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.9f, 0.8f, 0.8f, 0.4f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.9f, 0.65f, 0.65f, 0.45f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.27f, 0.27f, 0.54f, 0.83f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.4f, 0.4f, 0.8f, 0.2f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.32f, 0.32f, 0.63f, 0.87f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.4f, 0.4f, 0.55f, 0.8f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2f, 0.25f, 0.3f, 0.6f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.4f, 0.4f, 0.8f, 0.3f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.4f, 0.4f, 0.8f, 0.4f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.8f, 0.5f, 0.5f, 0.4f);
        style.Colors[ImGuiCol_ComboBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.99f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9f, 0.9f, 0.9f, 0.5f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 1.0f, 1.0f, 0.3f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8f, 0.5f, 0.5f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.67f, 0.4f, 0.4f, 0.6f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.67f, 0.4f, 0.4f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.8f, 0.5f, 0.5f, 1.0f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.4f, 0.4f, 0.9f, 0.45f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.45f, 0.9f, 0.8f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.87f, 0.8f);
        style.Colors[ImGuiCol_Column] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.7f, 0.6f, 0.6f, 1.0f);
        style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.9f, 0.7f, 0.7f, 1.0f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.0f, 1.0f, 1.0f, 0.3f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.6f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.9f);
        style.Colors[ImGuiCol_CloseButton] = ImVec4(0.5f, 0.5f, 0.9f, 0.5f);
        style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.7f, 0.7f, 0.9f, 0.6f);
        style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.9f, 0.7f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.9f, 0.7f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 0.0f, 1.0f, 0.35f);
        style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.2f, 0.2f, 0.2f, 0.35f);
    }
}
