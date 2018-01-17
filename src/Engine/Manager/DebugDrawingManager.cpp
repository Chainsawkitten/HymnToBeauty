#include "DebugDrawingManager.hpp"

#include "../Entity/World.hpp"
#include "../Component/Mesh.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "Managers.hpp"
#include <Video/RenderSurface.hpp>
#include <Video/Buffer/FrameBuffer.hpp>

#include "RenderManager.hpp"

using namespace Video;

DebugDrawingManager::DebugDrawingManager() {
    debugDrawing = new DebugDrawing();
}

DebugDrawingManager::~DebugDrawingManager() {
    delete debugDrawing;
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

void DebugDrawingManager::AddMesh(unsigned int id, Component::Mesh* meshComponent, const glm::mat4& matrix, const glm::vec3& color, bool wireFrame, float duration, bool depthTesting) {
    assert(meshComponent);
    assert(meshComponent->geometry);

    if (meshMap.find(id) == meshMap.end()) {
        DebugDrawing::Mesh mesh;
        if (meshComponent->geometry->GetVertexArray() == 0)
            debugDrawing->GenerateBuffers(meshComponent->geometry->GetVertexPositionData(), meshComponent->geometry->GetVertexIndexData(), mesh);
        else {
            mesh.vertexArray = meshComponent->geometry->GetVertexArray();
            mesh.indexCount = meshComponent->geometry->GetIndexCount();
        }  
        mesh.referenceCount = 0;
        meshMap[id] = mesh;
    }

    DebugDrawing::Mesh& mesh = meshMap[id];
    mesh.referenceCount++;
    mesh.matrix = matrix;
    mesh.color = color;
    mesh.wireFrame = wireFrame;
    mesh.duration = duration;
    mesh.depthTesting = depthTesting;
}

void DebugDrawingManager::Update(float deltaTime) {
    // Points.
    for (std::size_t i=0; i < points.size(); ++i) {
        if (points[i].duration < 0.f) {
            points[i] = points[points.size() - 1];
            points.pop_back();
            --i;
        } else
            points[i].duration -= deltaTime;
    }
    
    // Lines.
    for (std::size_t i=0; i < lines.size(); ++i) {
        if (lines[i].duration < 0.f) {
            lines[i] = lines[lines.size() - 1];
            lines.pop_back();
            --i;
        } else
            lines[i].duration -= deltaTime;
    }
    
    // Cuboids.
    for (std::size_t i=0; i < cuboids.size(); ++i) {
        if (cuboids[i].duration < 0.f) {
            cuboids[i] = cuboids[cuboids.size() - 1];
            cuboids.pop_back();
            --i;
        } else
            cuboids[i].duration -= deltaTime;
    }
    
    // Planes.
    for (std::size_t i=0; i < planes.size(); ++i) {
        if (planes[i].duration < 0.f) {
            planes[i] = planes[planes.size() - 1];
            planes.pop_back();
            --i;
        } else
            planes[i].duration -= deltaTime;
    }
    
    // Circles.
    for (std::size_t i=0; i < circles.size(); ++i) {
        if (circles[i].duration < 0.f) {
            circles[i] = circles[circles.size() - 1];
            circles.pop_back();
            --i;
        } else
            circles[i].duration -= deltaTime;
    }
    
    // Spheres.
    for (std::size_t i=0; i < spheres.size(); ++i) {
        if (spheres[i].duration < 0.f) {
            spheres[i] = spheres[spheres.size() - 1];
            spheres.pop_back();
            --i;
        } else
            spheres[i].duration -= deltaTime;
    }
    
    // Cylinders.
    for (std::size_t i=0; i < cylinders.size(); ++i) {
        if (cylinders[i].duration < 0.f) {
            cylinders[i] = cylinders[cylinders.size() - 1];
            cylinders.pop_back();
            --i;
        } else
            cylinders[i].duration -= deltaTime;
    }
    
    // Cone.
    for (std::size_t i=0; i < cones.size(); ++i) {
        if (cones[i].duration < 0.f) {
            cones[i] = cones[cones.size() - 1];
            cones.pop_back();
            --i;
        } else
            cones[i].duration -= deltaTime;
    }

    // Mesh.
    for (auto& it : meshMap) {
        DebugDrawing::Mesh& mesh = it.second;
        if (mesh.duration <= 0.f) {
            if (--mesh.referenceCount < 0)
                if (mesh.vertexBuffer || mesh.indexBuffer)
                    debugDrawing->DeleteBuffers(mesh);
        } else
            mesh.duration -= deltaTime;
    }
    for (auto it = meshMap.cbegin(); it != meshMap.cend();) {
        if (it->second.referenceCount < 0)
            meshMap.erase(it++);
        else
            ++it;
    }
}

void DebugDrawingManager::Render(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, Video::RenderSurface* renderSurface) {
    // Bind render target.
    renderSurface->GetShadingFrameBuffer()->BindWrite();
    debugDrawing->StartDebugDrawing(projectionMatrix * viewMatrix);
        
    // Points.
    for (const DebugDrawing::Point& point : points)
        debugDrawing->DrawPoint(point);
        
    // Lines.
    for (const DebugDrawing::Line& line : lines)
        debugDrawing->DrawLine(line);
        
    // Cuboids.
    for (const DebugDrawing::Cuboid& cuboid : cuboids)
        debugDrawing->DrawCuboid(cuboid);
       
    // Planes.
    for (const DebugDrawing::Plane& plane : planes)
        debugDrawing->DrawPlane(plane);
    
    // Circles.
    for (const DebugDrawing::Circle& circle : circles)
        debugDrawing->DrawCircle(circle);
    
    // Spheres.
    for (const DebugDrawing::Sphere& sphere : spheres)
        debugDrawing->DrawSphere(sphere);
    
    // Cylinders.
    for (const DebugDrawing::Cylinder& cylinder : cylinders)
        debugDrawing->DrawCylinder(cylinder);
    
    // Cones.
    for (const DebugDrawing::Cone& cone : cones)
        debugDrawing->DrawCone(cone);

    // Meshes.
    for (const std::pair<unsigned int, DebugDrawing::Mesh>& it : meshMap)
        debugDrawing->DrawMesh(it.second);
    
    debugDrawing->EndDebugDrawing();
    renderSurface->GetShadingFrameBuffer()->Unbind();
}
