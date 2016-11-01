#pragma once

class Entity;
class ShaderProgram;

namespace Component {
    class Mesh;
}

#include <glm/glm.hpp>

/// Render program to render an entity using skin shader program.
class SkinRenderProgram {
    public:
        /// Create new default render program.
		/**
		 * @param shaderProgram A GLSL shader program.
		 */
        SkinRenderProgram(ShaderProgram* shaderProgram);
        
        /// Destructor.
        ~SkinRenderProgram();
        
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
