#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Geometry/Geometry3D.hpp"

namespace Video {
    class ShaderProgram;

    /// Draws debug primitives.
    class DebugDrawing {
        public:
            /// A debug drawing point.
            struct Point {
                /// World position.
                glm::vec3 position;

                /// Color.
                glm::vec3 color;

                /// Size.
                float size;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;
            };

            /// A debug drawing line.
            struct Line {
                /// Starting position of the line.
                glm::vec3 startPosition;

                /// End position of the line.
                glm::vec3 endPosition;

                /// Color.
                glm::vec3 color;

                /// Line width.
                float width;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;
            };

            /// A debug drawing cuboid.
            struct Cuboid {
                /// The dimensions of the cuboid.
                glm::vec3 dimensions;

                /// The matrix used to transform the cuboid.
                glm::mat4 matrix;

                /// Color.
                glm::vec3 color;

                /// Line width.
                float lineWidth;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;
            };

            /// A debug drawing plane.
            struct Plane {
                /// The center position of the plane.
                glm::vec3 position;

                /// The plane normal.
                glm::vec3 normal;

                /// Size.
                glm::vec2 size;

                /// Color.
                glm::vec3 color;

                /// Line width.
                float lineWidth;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;
            };

            /// A debug drawing circle.
            struct Circle {
                /// The center position of the circle.
                glm::vec3 position;

                /// The circle normal.
                glm::vec3 normal;

                /// Radius.
                float radius;

                /// Color.
                glm::vec3 color;

                /// Line width.
                float lineWidth;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;
            };

            /// A debug drawing sphere.
            struct Sphere {
                /// The center position of the sphere.
                glm::vec3 position;

                /// Radius.
                float radius;

                /// Color.
                glm::vec3 color;

                /// Line width.
                float lineWidth;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;
            };

            /// A debug drawing cylinder.
            struct Cylinder {
                /// Radius.
                float radius;

                /// Length.
                float length;

                /// The matrix used to transform the cylinder.
                glm::mat4 matrix;

                /// Color.
                glm::vec3 color;

                /// Line width.
                float lineWidth;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;
            };

            /// A debug drawing cone.
            struct Cone {
                /// Radius.
                float radius;

                /// Height.
                float height;

                /// The matrix used to transform the cone.
                glm::mat4 matrix;

                /// Color.
                glm::vec3 color;

                /// Line width.
                float lineWidth;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;
            };

            /// A debug drawing mesh.
            struct Mesh {
                /// Reference count.
                int referenceCount;

                /// The matrix used to transform the cone.
                glm::mat4 matrix;

                /// Color.
                glm::vec3 color;

                /// Draw wire framed.
                bool wireFrame;

                /// Duration (in seconds).
                float duration;

                /// Whether to enable depth testing.
                bool depthTesting;

                /// Index count.
                unsigned int indexCount = 0;

                /// Vertex buffer.
                GLuint vertexBuffer = 0;

                /// Index buffer.
                GLuint indexBuffer = 0;

                /// Vertex array.
                GLuint vertexArray = 0;
            };

            /// Create new debug primitive renderer.
            DebugDrawing();

            /// Destructor.
            ~DebugDrawing();

            /// Start rendering debug primitives.
            /**
             * Needs to be called before DrawPoint, DrawLine or DrawCuboid.
             * @param viewProjectionMatrix The camera's view projection matrix.
             */
            void StartDebugDrawing(const glm::mat4& viewProjectionMatrix);

            /// Draw a point.
            /**
             * @param point The point to draw.
             */
            void DrawPoint(const Point& point);

            /// Draw a line.
            /**
             * @param line The line to draw.
             */
            void DrawLine(const Line& line);

            /// Draw a cuboid.
            /**
             * @param cuboid The cuboid to draw.
             */
            void DrawCuboid(const Cuboid& cuboid);

            /// Draw a plane.
            /**
             * @param plane The plane to draw.
             */
            void DrawPlane(const Plane& plane);

            /// Draw a circle.
            /**
             * @param circle The circle to draw.
             */
            void DrawCircle(const Circle& circle);

            /// Draw a sphere.
            /**
             * @param sphere The sphere to draw.
             */
            void DrawSphere(const Sphere& sphere);

            /// Draw a cylinder.
            /**
             * @param cylinder The cylinder to draw.
             */
            void DrawCylinder(const Cylinder& cylinder);

            /// Draw a cone.
            /**
             * @param cone The cone to draw.
             */
            void DrawCone(const Cone& cone);

            /// Draw a mesh.
            /**
             * @param mesh The mesh to draw.
             */
            void DrawMesh(const Mesh& mesh);

            /// Generate mesh buffers.
            /**
             * @param vertices Vector of vertices.
             * @param indices Vector of indices.
             * @param mesh The mesh containing the buffers.
             */
            void GenerateBuffers(const std::vector<glm::vec3>& vertices, const std::vector<unsigned int>& indices, Mesh& mesh);

            /// Delete mesh buffers.
            /**
             * @param mesh The mesh containing the buffers.
             */
            void DeleteBuffers(Mesh& mesh);

            /// Stop debug drawing.
            void EndDebugDrawing();

        private:
            DebugDrawing(const DebugDrawing & other) = delete;

            static void CreateVertexArray(const glm::vec3* positions, unsigned int positionCount, GLuint& vertexBuffer, GLuint& vertexArray);
            void BindVertexArray(GLuint vertexArray);
            void CreateCircle(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail);
            void CreateSphere(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail);
            void CreateCylinder(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail);
            void CreateCone(glm::vec3*& positions, unsigned int& vertexCount, unsigned int detail);

            Video::ShaderProgram* shaderProgram;

            // Uniform locations.
            GLuint viewProjectionLocation;
            GLuint modelLocation;
            GLuint colorLocation;
            GLuint sizeLocation;

            // Geometry.
            GLuint pointVertexBuffer;
            GLuint pointVertexArray;

            GLuint lineVertexBuffer;
            GLuint lineVertexArray;

            GLuint cuboidVertexBuffer;
            GLuint cuboidVertexArray;

            GLuint planeVertexBuffer;
            GLuint planeVertexArray;

            GLuint sphereVertexBuffer;
            GLuint sphereVertexArray;
            unsigned int sphereVertexCount;

            GLuint circleVertexBuffer;
            GLuint circleVertexArray;
            unsigned int circleVertexCount;

            GLuint cylinderVertexBuffer;
            GLuint cylinderVertexArray;
            unsigned int cylinderVertexCount;

            GLuint coneVertexBuffer;
            GLuint coneVertexArray;
            unsigned int coneVertexCount;

            GLuint boundVertexArray = 0;
    };
}
