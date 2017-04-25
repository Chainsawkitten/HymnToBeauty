#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;
class ShaderProgram;
class World;

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
        
        /// Update the debug geometry.
        /**
         * @param deltaTime Time since last frame (in seconds).
         */
        void Update(float deltaTime);
        
        /// Render the debug primitives.
        /**
         * @param world Contains a camera through which to render.
         */
        void Render(World& world);
        
    private:
        DebugDrawingManager();
        ~DebugDrawingManager();
        DebugDrawingManager(DebugDrawingManager const&) = delete;
        void operator=(DebugDrawingManager const&) = delete;
        
        Shader* vertexShader;
        Shader* fragmentShader;
        ShaderProgram* shaderProgram;
        
        // Points.
        struct Point {
            glm::vec3 position;
            glm::vec3 color;
            float size;
            float duration;
            bool depthTesting;
        };
        std::vector<Point> points;
        
        GLuint pointVertexBuffer;
        GLuint pointVertexArray;
        
        // Lines.
        struct Line {
            glm::vec3 startPosition;
            glm::vec3 endPosition;
            glm::vec3 color;
            float width;
            float duration;
            bool depthTesting;
        };
        std::vector<Line> lines;
};
