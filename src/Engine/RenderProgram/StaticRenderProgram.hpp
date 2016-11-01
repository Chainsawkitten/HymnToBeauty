#pragma once

class Entity;
class ShaderProgram;

namespace Component {
    class Mesh;
}

#include <glm/glm.hpp>

/// Render program to render an entity using default shader program.
class StaticRenderProgram {
    public:
        /// Create new default render program.
		/**
		 * @param shaderProgram A GLSL shader program.
		 */
        StaticRenderProgram(ShaderProgram* shaderProgram);
        
        /// Destructor.
        ~StaticRenderProgram();
        
        /// Bind render program.
        void PreRender(Entity* camera, glm::vec2& screenSize);

        /// Render mesh.
        /**
         * @param mesh Mesh to render.
         */
        void Render(Component::Mesh* mesh) const;

        /// Unbind render program.
        void PostRender() const;
        
    private:
        ShaderProgram* shaderProgram;

        glm::mat4 viewMat;
        glm::mat4 projectionMat;
        glm::mat4 viewProjectionMat;
};
