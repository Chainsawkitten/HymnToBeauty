#include "BezierWidget.hpp"
#include <imgui_internal.h>
#include <Engine/Util/Log.hpp>
#define NOMINMAX
#include <limits>
#include <algorithm>

bool compareVec3x(glm::vec2 a, glm::vec2 b){
    return (a.x < b.x);
}

Curve::Curve() {
    points.push_back(glm::vec2(0.f, 0.25f));
    points.push_back(glm::vec2(0.25f, 0.35f));
    points.push_back(glm::vec2(0.50f, 1.0f));
    points.push_back(glm::vec2(0.75f, 0.35f));
    points.push_back(glm::vec2(1.0f, 0.00f));
}

void Curve::AddPoint(glm::vec2 newPoint) {
    auto iterator = points.begin();
    
    for(int i = 0; i < points.size() && points[i].x < newPoint.x; i++)
        iterator++;
    points.insert(iterator++, newPoint);
}

int Curve::GetClosestPointIndex(glm::vec2 point){
    float closestDistance = std::numeric_limits<float>::max();
    int indexOfClosestPoint = 0;
    
    for(int i = 0; i < points.size(); i++){
        float a = points[i].x - point.x;
        float b = points[i].y - point.y;
        float distance = glm::sqrt(a*a + b*b);
        if(distance < closestDistance){
            closestDistance = distance;
            indexOfClosestPoint = i;
        }
    }
    return indexOfClosestPoint;
}

void Curve::StickToEdges(){
    points[0].x = 0.f;
    points[points.size()-1].x = 1.f;
}

void Curve::Sort(){
    std::sort(points.begin(), points.end(), compareVec3x);
}

BezierWidget::BezierWidget(ImVec2 size, Curve* curve){
    this->size = size;
    this->curve = curve;
}

void BezierWidget::Show() {
    // Setup.
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
        window->DrawList->AddBezierCurve(current, ImVec2(current.x + 18.f, current.y), ImVec2(next.x - 18.f, next.y), next, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered), 1.5f);
    }
    
    // Ensure that the mouse is inside the rectangle.
    if(hovered){
        // Render control points.
        for(int i = 0; i < curve->points.size()-1; i++) {
            ImVec2 current = ImVec2(bb.Min.x + curve->points[i].x*size.x, bb.Max.y - curve->points[i].y*size.y);
            ImVec2 topRight = ImVec2(current.x + 2.f, current.y + 2.f);
            ImVec2 bottomLeft = ImVec2(current.x - 2.f, current.y - 2.f);
            window->DrawList->AddRect(topRight, bottomLeft, ImGui::GetColorU32(ImGuiCol_PlotLinesHovered));
        }
        ImVec2 position = ImVec2((context.IO.MousePos.x - bb.Min.x)/(bb.Max.x - bb.Min.x), (context.IO.MousePos.y - bb.Min.y)/(bb.Max.y - bb.Min.y));
        position.y = 1.f - position.y;
        std::string info = "X: " + std::to_string(position.x) + " Y: " + std::to_string(position.y);
        
        // Render position to screen.
        ImGui::RenderTextClipped(ImVec2(bb.Min.x, bb.Min.y + style.FramePadding.y ), bb.Max, info.c_str(), NULL, NULL);
        
        // Get the index of the point closest to the mouse.
        int index = curve->GetClosestPointIndex(glm::vec2(position.x, position.y));
        
        if(context.IO.MouseDown[0]){
            // LMB: add a point.
            if(curve->points.size() < 10)
                curve->AddPoint(glm::vec2(position.x, position.y));
        } else if (context.IO.MouseDown[1]){
            // RMB: Move a point.
            curve->points[index] = glm::vec2(position.x, position.y);
            curve->StickToEdges();
            curve->Sort();
        } else if(ImGui::IsKeyPressed(261)){
            // Delete: remove a point.
            if(curve->points.size() > 2){
                curve->points.erase(curve->points.begin() + index);
                curve->StickToEdges();
                curve->Sort();
            }
        }
    }
}
