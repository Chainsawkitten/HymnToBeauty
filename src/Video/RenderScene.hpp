#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "Culling/AxisAlignedBoundingBox.hpp"
#include "DebugDrawing.hpp"

namespace Video {

namespace Geometry {
class Geometry3D;
}
class Texture2D;

/// Contains all data needed to render a scene.
struct RenderScene {
    /// A directional light source (like the sun).
    struct DirectionalLight {
        glm::vec3 direction;
        glm::vec3 color;
        float ambientCoefficient;
    };

    /// The directional lights in the scene.
    std::vector<DirectionalLight> directionalLights;

    /// A point light.
    struct PointLight {
        glm::vec3 position;
        glm::vec3 color;
        float distance;
        float attenuation;
        float intensity;
    };

    /// The point lights in the scene.
    std::vector<PointLight> pointLights;

    /// A spot light.
    struct SpotLight {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 color;
        float distance;
        float attenuation;
        float intensity;
        float coneAngle;
    };

    /// The spot lights in the scene.
    std::vector<SpotLight> spotLights;

    /// A camera through which the scene should be rendered.
    struct Camera {
        glm::vec3 position;
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
        glm::mat4 viewProjectionMatrix;
        float zNear;
        float zFar;
		glm::vec4 viewport;
        uint32_t layerMask;

        /// Configuration of post-processing effects.
        Video::PostProcessing::Configuration postProcessingConfiguration;
    };

    /// The cameras through which to render the scene.
    std::vector<Camera> cameras;

    /// A mesh that should be rendered.
    struct Mesh {
        glm::mat4 modelMatrix;
        AxisAlignedBoundingBox axisAlignedBoundingBox;
        Geometry::Geometry3D* geometry;
        Texture2D* albedo;
        Texture2D* normal;
        Texture2D* roughnessMetallic;
        uint32_t layerMask;
    };

    /// The meshes that should be rendered.
    std::vector<Mesh> meshes;

    /// A billboarded texture.
    struct Icon {
        Texture2D* texture;
        std::vector<glm::vec3> positions;
    };

    /// The icons that should be rendered.
    std::vector<Icon> icons;

    /// Debug drawing points.
    std::vector<DebugDrawing::Point> debugDrawingPoints;

    /// Debug drawing lines.
    std::vector<DebugDrawing::Line> debugDrawingLines;

    /// Debug drawing cuboids.
    std::vector<DebugDrawing::Cuboid> debugDrawingCuboids;

    /// Debug drawing planes.
    std::vector<DebugDrawing::Plane> debugDrawingPlanes;

    /// Debug drawing circles.
    std::vector<DebugDrawing::Circle> debugDrawingCircles;

    /// Debug drawing spheres.
    std::vector<DebugDrawing::Sphere> debugDrawingSpheres;

    /// Debug drawing cylinders.
    std::vector<DebugDrawing::Cylinder> debugDrawingCylinders;

    /// Debug drawing cones.
    std::vector<DebugDrawing::Cone> debugDrawingCones;

    /// Debug drawing meshes.
    std::vector<DebugDrawing::Mesh> debugDrawingMeshes;

    ///  A sprite that should be rendered.
    struct Sprite {
        Texture2D* texture;
        glm::mat4 modelMatrix;
        glm::vec2 size;
        glm::vec2 pivot;
        glm::vec4 tint;
        uint32_t layerMask;
    };

    std::vector<Sprite> sprites;
};

} // namespace Video
