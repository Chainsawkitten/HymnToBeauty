#include "DebugDrawingManager.hpp"

DebugDrawingManager::DebugDrawingManager() {
    
}

DebugDrawingManager::~DebugDrawingManager() {
    
}

void DebugDrawingManager::AddPoint(const glm::vec3& position, const glm::vec3& color, float duration, bool depthTesting) {
    Point point;
    point.position = position;
    point.color = color;
    point.duration = duration;
    point.depthTesting = depthTesting;
    points.push_back(point);
}
