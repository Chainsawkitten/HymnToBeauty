#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <Video/DebugDrawing.hpp>

namespace Component {
class Mesh;
}

/// Debug drawing facilities.
class DebugDrawingManager {
    friend class Hub;

  public:
    /// Add a point to the world.
    /**
     * @param position World position of the point.
     * @param color Color of the point.
     * @param size Size of the point.
     * @param duration How long the point should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddPoint(const glm::vec3& position, const glm::vec3& color, float size, float duration = 0.f, bool depthTesting = true);

    /// Get all debug points.
    /**
     * @return All debug points.
     */
    const std::vector<Video::DebugDrawing::Point>& GetPoints() const;

    /// Add a line to the world.
    /**
     * @param startPosition Position of the start point of the line.
     * @param endPosition Position of the end point of the line.
     * @param color Color of the line.
     * @param width The width of the line.
     * @param duration How long the line should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddLine(const glm::vec3& startPosition, const glm::vec3& endPosition, const glm::vec3& color, float width = 1.f, float duration = 0.f, bool depthTesting = true);

    /// Get all debug lines.
    /**
     * @return All debug lines.
     */
    const std::vector<Video::DebugDrawing::Line>& GetLines() const;

    /// Add a cuboid to the world.
    /**
     * @param dimensions The dimensions of the cuboid.
     * @param matrix Matrix used to transform the cuboid.
     * @param color Color of the lines.
     * @param lineWidth The width of the lines used to draw the box.
     * @param duration How long the box should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddCuboid(const glm::vec3& dimensions, const glm::mat4& matrix, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);

    /// Get all debug cuboids.
    /**
     * @return All debug cuboids.
     */
    const std::vector<Video::DebugDrawing::Cuboid>& GetCuboids() const;

    /// Add a plane to the world.
    /**
     * @param position Center position of the plane.
     * @param normal Plane normal.
     * @param size Size.
     * @param color Color of the lines.
     * @param lineWidth The width of the lines used to draw the plane.
     * @param duration How long the plane should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddPlane(const glm::vec3& position, const glm::vec3& normal, const glm::vec2& size, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);

    /// Get all debug planes.
    /**
     * @return All debug planes.
     */
    const std::vector<Video::DebugDrawing::Plane>& GetPlanes() const;

    /// Add a circle to the world.
    /**
     * @param position Center position of the circle.
     * @param normal Circle normal.
     * @param radius Radius.
     * @param color Color of the lines.
     * @param lineWidth The width of the lines used to draw the circle.
     * @param duration How long the circle should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddCircle(const glm::vec3& position, const glm::vec3& normal, float radius, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);

    /// Get all debug circles.
    /**
     * @return All debug circles.
     */
    const std::vector<Video::DebugDrawing::Circle>& GetCircles() const;

    /// Add a sphere to the world.
    /**
     * @param position The position of the sphere.
     * @param radius The radius of the sphere.
     * @param color Color of the lines.
     * @param lineWidth The width of the lines used to draw the sphere.
     * @param duration How long the sphere should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddSphere(const glm::vec3& position, float radius, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);

    /// Get all debug spheres.
    /**
     * @return All debug spheres.
     */
    const std::vector<Video::DebugDrawing::Sphere>& GetSpheres() const;

    /// Add a cylinder to the world.
    /**
     * @param radius The radius of the cylinder.
     * @param length The length of the cylinder.
     * @param matrix Matrix used to transform the cylinder.
     * @param color Color of the lines.
     * @param lineWidth The width of the lines used to draw the cylinder.
     * @param duration How long the cylinder should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddCylinder(float radius, float length, const glm::mat4& matrix, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);

    /// Get all debug cylinders.
    /**
     * @return All debug cylinders.
     */
    const std::vector<Video::DebugDrawing::Cylinder>& GetCylinders() const;

    /// Add a cone to the world.
    /**
     * @param radius The radius of the cone.
     * @param height The height of the cone.
     * @param matrix Matrix to transform the cone with.
     * @param color Color of the lines.
     * @param lineWidth The width of the lines used to draw the cone.
     * @param duration How long the cone should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddCone(float radius, float height, const glm::mat4& matrix, const glm::vec3& color, float lineWidth = 1.f, float duration = 0.f, bool depthTesting = true);

    /// Get all debug cones.
    /**
     * @return All debug cones.
     */
    const std::vector<Video::DebugDrawing::Cone>& GetCones() const;

    /// Add a mesh to the world.
    /**
     * @param meshComponent The mesh component.
     * @param matrix Matrix to transform the mesh with.
     * @param color Color of the lines.
     * @param wireFrame Whether to wireframe the mesh.
     * @param duration How long the mesh should stay in the world (in seconds).
     * @param depthTesting Whether to enable depth testing.
     */
    void AddMesh(Component::Mesh* meshComponent, const glm::mat4& matrix, const glm::vec3& color, bool wireFrame = true, float duration = 0.f, bool depthTesting = true);

    /// Get all debug meshes.
    /**
     * @return All debug meshes.
     */
    std::vector<Video::DebugDrawing::Mesh> GetMeshes() const;

    /// Update the debug geometry.
    /**
     * @param deltaTime Time since last frame (in seconds).
     */
    void Update(float deltaTime);

  private:
    DebugDrawingManager();
    ~DebugDrawingManager();
    DebugDrawingManager(DebugDrawingManager const&) = delete;
    void operator=(DebugDrawingManager const&) = delete;

    std::vector<Video::DebugDrawing::Point> points;
    std::vector<Video::DebugDrawing::Line> lines;
    std::vector<Video::DebugDrawing::Cuboid> cuboids;
    std::vector<Video::DebugDrawing::Plane> planes;
    std::vector<Video::DebugDrawing::Circle> circles;
    std::vector<Video::DebugDrawing::Sphere> spheres;
    std::vector<Video::DebugDrawing::Cylinder> cylinders;
    std::vector<Video::DebugDrawing::Cone> cones;
    std::vector<Video::DebugDrawing::Mesh> meshes;
};
