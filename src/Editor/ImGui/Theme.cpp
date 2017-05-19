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
}
