#include "BezierWidget.hpp"
#include <imgui_internal.h>

Curve::Curve(){
    points.push_back(glm::vec2(0.f, 0.25f));
    points.push_back(glm::vec2(0.25f, 0.35f));
    points.push_back(glm::vec2(0.50f, 1.0f));
    points.push_back(glm::vec2(0.75f, 0.35f));
    points.push_back(glm::vec2(1.0f, 0.00f));
}

BezierWidget::BezierWidget(ImVec2 size, Curve& curve){
    this->size = size;
    this->curve = &curve;
}

void BezierWidget::show() {
    // Setup.
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    ImGuiContext& context = *GImGui;
    const ImGuiID id = window->GetID("Bezier Editor");
    const ImGuiStyle& style = context.Style;

    // Draw background.
    ImRect bb(window->DC.CursorPos, ImVec2(window->DC.CursorPos.x + size.x, window->DC.CursorPos.y + size.y));
    ImGui::ItemSize(bb);
    ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);
    const bool hovered = ImGui::IsHovered(bb, id);

    // Height and width 
    float height = bb.Max.y - bb.Min.y;
    float width = bb.Max.x - bb.Min.x;    
 
    // Draw reference lines along x/y axis.
    for(int i = 0; i < 9; i++) {
        window->DrawList->AddLine(
                    ImVec2(bb.Min.x + (width / 10) * (i + 1), bb.Min.y),
                    ImVec2(bb.Min.x + (width / 10) * (i + 1), bb.Max.y),
                    ImGui::GetColorU32(ImGuiCol_TextDisabled)
                    );
        window->DrawList->AddLine(
                    ImVec2(bb.Min.x, bb.Min.y + (height / 10)*(i + 1)),
                    ImVec2(bb.Max.x, bb.Min.y + (height / 10)*(i + 1)),
                    ImGui::GetColorU32(ImGuiCol_TextDisabled)
                    );
    }
    
    // Draw straight and bezier curve. TODO: Different ease functions.
    for(int i = 0; i < curve->points.size()-1; i++){
        ImVec2 current = ImVec2(bb.Min.x + curve->points[i].x*size.x, bb.Max.y - curve->points[i].y*size.y);
        ImVec2 next = ImVec2(bb.Min.x + curve->points[i+1].x*size.x, bb.Max.y - curve->points[i+1].y*size.y);
        
        // Straight lines for reference.
        window->DrawList->AddLine(
                    current,
                    next,
                    ImGui::GetColorU32(ImGuiCol_PlotLines)
                    );
        
        // Bezier curves
        window->DrawList->AddBezierCurve(current, ImVec2(current.x + 18.f, current.y), ImVec2(next.x - 18.f, next.y), next, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered), 2.f);
    }
    
    if(hovered){
        for(int i = 0; i < curve->points.size()-1; i++) {
            ImVec2 current = ImVec2(bb.Min.x + curve->points[i].x*size.x, bb.Max.y - curve->points[i].y*size.y);
            ImVec2 topRight = ImVec2(current.x + 2.f, current.y + 2.f);
            ImVec2 bottomLeft = ImVec2(current.x - 2.f, current.y - 2.f);
            window->DrawList->AddRect(topRight, bottomLeft, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        }
    }
}
