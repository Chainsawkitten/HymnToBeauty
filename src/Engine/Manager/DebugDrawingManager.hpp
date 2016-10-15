#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Shader;
class ShaderProgram;
class Scene;

/// Debug drawing facilities.
class DebugDrawingManager {
    friend class Hub;
    
    public:
        /// Add a point to the scene.
        /**
         * @param position World position of the point.
         * @param color Color of the point.
         * @param size Size of the point.
         * @param duration How long the point should stay in the scene (in seconds).
         * @param depthTesting Whether to enable depth testing.
         */
        void AddPoint(const glm::vec3& position, const glm::vec3& color, float size, float duration = 0.f, bool depthTesting = true);
        
        /// Update the debug geometry.
        /**
         * @param deltaTime Time since last frame (in seconds).
         */
        void Update(float deltaTime);
        
        /// Render the debug scene.
        /**
         * @param scene Contains a camera through which to render.
         */
        void Render(Scene& scene);
        
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
};
