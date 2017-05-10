#include "BezierWidget.hpp"
#include <imgui_internal.h>
#include <Engine/Util/Log.hpp>
#define NOMINMAX
#include <limits>
#include <algorithm>
#include <glm/gtc/constants.hpp>

#           define bounceout(p) ( \
                (p) < 4/11.0 ? (121 * (p) * (p))/16.0 : \
                (p) < 8/11.0 ? (363/40.0 * (p) * (p)) - (99/10.0 * (p)) + 17/5.0 : \
                (p) < 9/10.0 ? (4356/361.0 * (p) * (p)) - (35442/1805.0 * (p)) + 16061/1805.0 \
                           : (54/5.0 * (p) * (p)) - (513/25.0 * (p)) + 268/25.0 )

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

float Curve::Sample(float x){
    // If we sample outside, return edge y.
    if(x < 0.f)
        return points[0].y;
    else if(x > 1.f)
        return points[points.size()-1].y;

    int numberOfPoints = points.size();
    int leftIndex = 0;
    int rightIndex = 0;

    // Find the left and right indices.
    for(int i = 0; i < numberOfPoints; i++) {
        if(x >= points[i].x && x <= points[i+1].x){
            leftIndex = i;
            rightIndex = leftIndex+1;
            break;
        }

    }

    const float pi = glm::pi<float>();
    const float piHalf = glm::pi<float>() / 2.f;
    float rightCoefficient = (x - points[leftIndex].x) / (points[rightIndex].x - points[leftIndex].x);
    float leftCoefficient = 1.f - rightCoefficient;

    switch (curveType) {
        case CurveType::LINEAR:
            return leftCoefficient*points[leftIndex].y + rightCoefficient*points[rightIndex].y;
        break;

        case CurveType::QUADIN:
            leftCoefficient = leftCoefficient*leftCoefficient;
            rightCoefficient = 1.f-leftCoefficient;
            return leftCoefficient*points[leftIndex].y + rightCoefficient*points[rightIndex].y;
        break;

        case CurveType::QUADOUT:
            leftCoefficient = -leftCoefficient*(leftCoefficient-2.f);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient*points[leftIndex].y + rightCoefficient*points[rightIndex].y;
        break;

        case CurveType::QUADINOUT:
            if(leftCoefficient < 0.5f) {
                leftCoefficient = 2*leftCoefficient*leftCoefficient;
            } else {
                leftCoefficient = (-2.f * leftCoefficient * leftCoefficient) + (4*leftCoefficient) - 1.f;
            }
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient*points[leftIndex].y + rightCoefficient*points[rightIndex].y;
        break;

        case CurveType::CUBICIN:
            leftCoefficient = leftCoefficient * leftCoefficient * leftCoefficient;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient*points[leftIndex].y + rightCoefficient*points[rightIndex].y;
        break;

        case CurveType::CUBICOUT:
            leftCoefficient = leftCoefficient - 1.f;
            leftCoefficient = leftCoefficient * leftCoefficient * leftCoefficient + 1.f;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient*points[leftIndex].y + rightCoefficient*points[rightIndex].y;
        break;

        case CurveType::CUBICINOUT:
            if(leftCoefficient < 0.5f) {
                leftCoefficient = 4 * leftCoefficient * leftCoefficient * leftCoefficient;
            } else {
                float f = ((2.f * leftCoefficient) - 2.f);
                leftCoefficient = 0.5f * f * f * f  + 1.f;
            }
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::QUARTIN:
            leftCoefficient = leftCoefficient*leftCoefficient*leftCoefficient*leftCoefficient;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::QUARTOUT: { // Due to case label.
            float f = leftCoefficient-1.f;
            leftCoefficient = f*f*f*(1.f - leftCoefficient) + 1.f;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        }
        break;

        case CurveType::QUARTINOUT:
            if(leftCoefficient < 0.5f) {
                leftCoefficient = 8.f*leftCoefficient*leftCoefficient*leftCoefficient*leftCoefficient;
            } else {
                float f = (leftCoefficient - 1.f);
                leftCoefficient = -8.f * f * f * f * f + 1.f;
            }
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::QUINTIN:
            leftCoefficient = leftCoefficient * leftCoefficient * leftCoefficient * leftCoefficient * leftCoefficient;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::QUINTOUT:
            leftCoefficient = (leftCoefficient - 1.f);
            leftCoefficient = leftCoefficient * leftCoefficient * leftCoefficient * leftCoefficient * leftCoefficient + 1.f;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::QUINTINOUT:
            if( leftCoefficient < 0.5f){
                leftCoefficient = 16 * leftCoefficient * leftCoefficient * leftCoefficient * leftCoefficient * leftCoefficient;
            } else {
                float f = ((2.f * leftCoefficient) - 2.f);
                leftCoefficient = 0.5f * f * f  * f * f * f + 1.f;
            }
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::SINEIN:
            leftCoefficient = std::sin((leftCoefficient - 1.f)*piHalf) + 1.f;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::SINEOUT:
            leftCoefficient = std::sin(leftCoefficient * piHalf);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::SINEINOUT:
            leftCoefficient = 0.5f  *(1.f - std::cos(leftCoefficient*pi));
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::EXPOIN:
            // Check if we are too close to zero.
            if( leftCoefficient < 0.0001f && leftCoefficient > -0.0001f ) {
                rightCoefficient = 1.f - leftCoefficient;
                return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
            } else {
                leftCoefficient = std::pow(2.f, 10.f * (leftCoefficient - 1.f));
                rightCoefficient = 1.f - leftCoefficient;
                return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
            }
        break;

        case CurveType::EXPOOUT:
            if( leftCoefficient < 1.0001f && leftCoefficient > (1.0001f - 0.0001f)  ) {
                rightCoefficient = 1.f - leftCoefficient;
                return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
            }
            leftCoefficient = 1.f - pow(2.f,-10*leftCoefficient);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::EXPOINOUT:
            if( (leftCoefficient < 1.0001f && leftCoefficient > (1.0001f - 0.0001f)) ||  (leftCoefficient < 0.0001f && leftCoefficient > -0.0001f ) ){
                rightCoefficient = 1.f - leftCoefficient;
                return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
            }

            if(leftCoefficient < 0.5f) {
                leftCoefficient = 0.5f * pow(2.f, (20.f*leftCoefficient) - 10.f);
            } else {
                leftCoefficient = -0.5f * pow(2.f, (-20.f*leftCoefficient) + 10.f) + 1.f;
            }
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::CIRCIN:
            leftCoefficient = 1.f - std::sqrt(1.f - (leftCoefficient*leftCoefficient));
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::CIRCOUT:
            leftCoefficient = std::sqrt((2.f - leftCoefficient) * leftCoefficient);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::CIRCINOUT:
            if(leftCoefficient < 0.5f){
                leftCoefficient = 0.5f * (1.f - sqrt(1.f - 4.f*(leftCoefficient*leftCoefficient)));
            } else {
                leftCoefficient = 0.5f * (sqrt(-1*((2*leftCoefficient)-3.f)*((2.f*leftCoefficient)-1.f)) + 1.f);
            }
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::ELASTICIN:
            leftCoefficient = std::sin(13.f * piHalf * leftCoefficient) * std::pow(2.f, 10.f * (leftCoefficient - 1.f));
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::ELASTICOUT:
            leftCoefficient = std::sin(-13.f * piHalf * (leftCoefficient + 1.f)) * std::pow(2.f, -10.f * leftCoefficient) + 1.f;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::ELASTICINOUT:
            if(leftCoefficient < 0.5f) {
                leftCoefficient = 0.5f * std::sin(13.f * piHalf * (2.f * leftCoefficient)) * std::pow(2.f, 10.f * ((2.f * leftCoefficient) - 1.f));
            } else {
                leftCoefficient = 0.5f * (std::sin(-13.f * piHalf * ((2.f * leftCoefficient - 1.f) + 1.f)) * std::pow(2.f, -10.f * (2.f * leftCoefficient - 1.f)) + 2.f);
            }
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::BACKIN: {
            const float s = 1.70158f;
            leftCoefficient = leftCoefficient * leftCoefficient * ((s + 1.f) * leftCoefficient - s);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        }
        break;

        case CurveType::BACKOUT: {
            const float s = 1.70158f;
            leftCoefficient -= 1.f;
            leftCoefficient = 1.f * (leftCoefficient*leftCoefficient*((s + 1.f)*leftCoefficient + s) + 1.f);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        }
        break;

        case CurveType::BACKINOUT:
            if(leftCoefficient < 0.5f){
                float f = 2*leftCoefficient;
                leftCoefficient = 0.5f * (f * f * f - f * std::sin(f * pi) );
                rightCoefficient = 1.f - leftCoefficient;
                return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
            } else {
                float f = (1.f - (2.f*leftCoefficient - 1.f));
                leftCoefficient = 0.5f * (1.f - (f * f * f - f * std::sin(f * pi))) + 0.5f;
                rightCoefficient = 1.f - leftCoefficient;
                return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
            }
        break;

        case CurveType::BOUNCEIN:
            leftCoefficient = 1.f - bounceout(1.f - leftCoefficient);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::BOUNCEOUT:
            leftCoefficient = bounceout(leftCoefficient);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;

        case CurveType::BOUNCEINOUT:
            if(leftCoefficient < 0.5f){
                leftCoefficient = 0.5f * (1.f - bounceout(1.f - leftCoefficient * 2.f));
                rightCoefficient = 1.f - leftCoefficient;
                return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
            } else {
                leftCoefficient = 0.5f * bounceout((leftCoefficient * 2.f -1.f)) + 0.5f;
                rightCoefficient = 1.f - leftCoefficient;
                return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
            }
        break;

        case CurveType::SINESQUARE:
            leftCoefficient = std::sin((leftCoefficient) * piHalf);
            leftCoefficient *= leftCoefficient;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;
    
        case CurveType::EXPONENTIAL:
            leftCoefficient = 1.f / (1.f + std::exp(6.f - 12.f * (leftCoefficient)));
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;
        
        case CurveType::SCHUBRING1:
            leftCoefficient = 2.f * (leftCoefficient +(0.5f - leftCoefficient) * std::abs(0.5f - leftCoefficient)) - 0.5f; 
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;
        
        case CurveType::SCHUBRING2:{
            float p1pass = 2.f * (leftCoefficient*(0.5f - leftCoefficient)*std::abs(0.5f - leftCoefficient)) - 0.5f;
            float p2pass = 2.f * (p1pass + (0.5f - p1pass) * std::abs(0.5f - p1pass)) - 0.5f;
            leftCoefficient = (p1pass + p2pass) / 2.f;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        }
        break;
        
        case CurveType::SCHUBRING3:{
            double p1pass= 2.f*(leftCoefficient+(0.5f-leftCoefficient)*std::abs(0.5f-leftCoefficient))-0.5f;
            double p2pass= 2.f*(p1pass+(0.5f-p1pass)*std::abs(0.5f-p1pass))-0.5f;
            leftCoefficient = p2pass;
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        }
        break;
        
        case CurveType::SWING:
            leftCoefficient = ((-std::cos(pi * leftCoefficient) * 0.5f) + 0.5f);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;
        
        case CurveType::SINPI2:
            leftCoefficient = std::sin(leftCoefficient * piHalf);
            rightCoefficient = 1.f - leftCoefficient;
            return leftCoefficient * points[leftIndex].y + rightCoefficient * points[rightIndex].y;
        break;
        
        default:
            return -1.f;
        break;
    }
    return -1.f;
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
