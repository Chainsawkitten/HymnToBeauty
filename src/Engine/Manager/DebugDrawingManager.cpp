#include "DebugDrawingManager.hpp"

#include "../Entity/World.hpp"
#include "../Component/Mesh.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "Managers.hpp"
#include <Video/LowLevelRenderer/Interface/VertexDescription.hpp>
#include "../Geometry/Model.hpp"

#include "RenderManager.hpp"

using namespace Video;

DebugDrawingManager::DebugDrawingManager() {
    
}

DebugDrawingManager::~DebugDrawingManager() {
    
}

void DebugDrawingManager::AddPoint(const glm::vec3& position, const glm::vec3& color, float size, float duration, bool depthTesting) {
    DebugDrawing::Point point;
    point.position = position;
    point.color = color;
    point.size = size;
    point.duration = duration;
    point.depthTesting = depthTesting;
    points.push_back(point);
}

const std::vector<Video::DebugDrawing::Point>& DebugDrawingManager::GetPoints() const {
    return points;
}

void DebugDrawingManager::AddLine(const glm::vec3& startPosition, const glm::vec3& endPosition, const glm::vec3& color, float width, float duration, bool depthTesting) {
    DebugDrawing::Line line;
    line.startPosition = startPosition;
    line.endPosition = endPosition;
    line.color = color;
    line.width = width;
    line.duration = duration;
    line.depthTesting = depthTesting;
    lines.push_back(line);
}

const std::vector<Video::DebugDrawing::Line>& DebugDrawingManager::GetLines() const {
    return lines;
}

void DebugDrawingManager::AddCuboid(const glm::vec3& dimensions, const glm::mat4& matrix, const glm::vec3& color, float lineWidth, float duration, bool depthTesting) {
    DebugDrawing::Cuboid cuboid;
    cuboid.dimensions = dimensions;
    cuboid.matrix = matrix;
    cuboid.color = color;
    cuboid.lineWidth = lineWidth;
    cuboid.duration = duration;
    cuboid.depthTesting = depthTesting;
    cuboids.push_back(cuboid);
}

const std::vector<Video::DebugDrawing::Cuboid>& DebugDrawingManager::GetCuboids() const {
    return cuboids;
}

void DebugDrawingManager::AddPlane(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& size, const glm::vec3& color, float lineWidth, float duration, bool depthTesting) {
    DebugDrawing::Plane plane;
    plane.position = position;
    plane.normal = normal;
    plane.size = size;
    plane.color = color;
    plane.lineWidth = lineWidth;
    plane.duration = duration;
    plane.depthTesting = depthTesting;
    planes.push_back(plane);
}

const std::vector<Video::DebugDrawing::Plane>& DebugDrawingManager::GetPlanes() const {
    return planes;
}

void DebugDrawingManager::AddCircle(const glm::vec3& position, const glm::vec3& normal, float radius, const glm::vec3& color, float lineWidth, float duration, bool depthTesting) {
    DebugDrawing::Circle circle;
    circle.position = position;
    circle.normal = normal;
    circle.radius = radius;
    circle.color = color;
    circle.lineWidth = lineWidth;
    circle.duration = duration;
    circle.depthTesting = depthTesting;
    circles.push_back(circle);
}

const std::vector<Video::DebugDrawing::Circle>& DebugDrawingManager::GetCircles() const {
    return circles;
}

void DebugDrawingManager::AddSphere(const glm::vec3& position, float radius, const glm::vec3& color, float lineWidth, float duration, bool depthTesting) {
    DebugDrawing::Sphere sphere;
    sphere.position = position;
    sphere.radius = radius;
    sphere.color = color;
    sphere.lineWidth = lineWidth;
    sphere.duration = duration;
    sphere.depthTesting = depthTesting;
    spheres.push_back(sphere);
}

const std::vector<Video::DebugDrawing::Sphere>& DebugDrawingManager::GetSpheres() const {
    return spheres;
}

void DebugDrawingManager::AddCylinder(float radius, float length, const glm::mat4& matrix, const glm::vec3& color, float lineWidth, float duration, bool depthTesting) {
    DebugDrawing::Cylinder cylinder;
    cylinder.radius = radius;
    cylinder.length = length;
    cylinder.matrix = matrix;
    cylinder.color = color;
    cylinder.lineWidth = lineWidth;
    cylinder.duration = duration;
    cylinder.depthTesting = depthTesting;
    cylinders.push_back(cylinder);
}

const std::vector<Video::DebugDrawing::Cylinder>& DebugDrawingManager::GetCylinders() const {
    return cylinders;
}

void DebugDrawingManager::AddCone(float radius, float height, const glm::mat4& matrix, const glm::vec3& color, float lineWidth, float duration, bool depthTesting) {
    DebugDrawing::Cone cone;
    cone.radius = radius;
    cone.height = height;
    cone.matrix = matrix;
    cone.color = color;
    cone.lineWidth = lineWidth;
    cone.duration = duration;
    cone.depthTesting = depthTesting;
    cones.push_back(cone);
}

const std::vector<Video::DebugDrawing::Cone>& DebugDrawingManager::GetCones() const {
    return cones;
}

void DebugDrawingManager::AddMesh(Component::Mesh* meshComponent, const glm::mat4& matrix, const glm::vec3& color, bool wireFrame, float duration, bool depthTesting) {
    assert(meshComponent);
    assert(meshComponent->model);

    if (meshComponent->model->GetGeometryBinding() == nullptr)
        return;

    DebugDrawing::Mesh mesh;
    mesh.geometryBinding = meshComponent->model->GetGeometryBinding();
    mesh.indexCount = meshComponent->model->GetIndexCount();
    mesh.matrix = matrix;
    mesh.color = color;
    mesh.wireFrame = wireFrame;
    mesh.duration = duration;
    mesh.depthTesting = depthTesting;
    meshes.push_back(mesh);
}

std::vector<Video::DebugDrawing::Mesh> DebugDrawingManager::GetMeshes() const {
    return meshes;
}

void DebugDrawingManager::Update(float deltaTime) {
    // Points.
    for (std::size_t i = 0; i < points.size(); ++i) {
        if (points[i].duration < 0.f) {
            points[i] = points[points.size() - 1];
            points.pop_back();
            --i;
        } else
            points[i].duration -= deltaTime;
    }

    // Lines.
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].duration < 0.f) {
            lines[i] = lines[lines.size() - 1];
            lines.pop_back();
            --i;
        } else
            lines[i].duration -= deltaTime;
    }

    // Cuboids.
    for (std::size_t i = 0; i < cuboids.size(); ++i) {
        if (cuboids[i].duration < 0.f) {
            cuboids[i] = cuboids[cuboids.size() - 1];
            cuboids.pop_back();
            --i;
        } else
            cuboids[i].duration -= deltaTime;
    }

    // Planes.
    for (std::size_t i = 0; i < planes.size(); ++i) {
        if (planes[i].duration < 0.f) {
            planes[i] = planes[planes.size() - 1];
            planes.pop_back();
            --i;
        } else
            planes[i].duration -= deltaTime;
    }

    // Circles.
    for (std::size_t i = 0; i < circles.size(); ++i) {
        if (circles[i].duration < 0.f) {
            circles[i] = circles[circles.size() - 1];
            circles.pop_back();
            --i;
        } else
            circles[i].duration -= deltaTime;
    }

    // Spheres.
    for (std::size_t i = 0; i < spheres.size(); ++i) {
        if (spheres[i].duration < 0.f) {
            spheres[i] = spheres[spheres.size() - 1];
            spheres.pop_back();
            --i;
        } else
            spheres[i].duration -= deltaTime;
    }

    // Cylinders.
    for (std::size_t i = 0; i < cylinders.size(); ++i) {
        if (cylinders[i].duration < 0.f) {
            cylinders[i] = cylinders[cylinders.size() - 1];
            cylinders.pop_back();
            --i;
        } else
            cylinders[i].duration -= deltaTime;
    }

    // Cone.
    for (std::size_t i = 0; i < cones.size(); ++i) {
        if (cones[i].duration < 0.f) {
            cones[i] = cones[cones.size() - 1];
            cones.pop_back();
            --i;
        } else
            cones[i].duration -= deltaTime;
    }

    // Mesh.
    for (std::size_t i = 0; i < meshes.size(); ++i) {
        if (meshes[i].duration < 0.f) {
            meshes[i] = meshes[meshes.size() - 1];
            meshes.pop_back();
            --i;
        } else
            meshes[i].duration -= deltaTime;
    }
}
