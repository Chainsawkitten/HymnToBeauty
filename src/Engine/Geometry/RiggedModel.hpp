#pragma once

#include "Model.hpp"
#include "Animation.hpp"
#include "Skeleton.hpp"
#include "VertexType/SkinVertex.hpp"

struct aiNode;

namespace Geometry {
    /// A model loaded from a file.
    class RiggedModel : public Model {
        public:
            /// Create new empty rigged model.
            /**
             * The created rigged model has to be loaded later using Load.
             */
            RiggedModel();

            /// Create new rigged model from file.
            /**
             * @param filename Filename (relative or absolute) to the model file.
             */
            RiggedModel(const char* filename);

            /// Destructor.
            ~RiggedModel() final;

            /// Load rigged model from file.
            /**
             * @param filename Filename (relative or absolute) to the model file.
             */
            void Load(const char* filename);

            /// Get geometry type.
            /**
             * @return Type.
             */
            const Type GetType() const;

            Skeleton skeleton;
            std::vector<Animation> animations;

        protected:
            /// Generate vertex buffer.
            void GenerateVertexBuffer(GLuint& vertexBuffer);

            /// Generate vertex array.
            void GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray);

        private:
            struct MeshEntry {
                unsigned int numIndices = 0;
                unsigned int baseVertex = 0;
                unsigned int baseIndex = 0;
            };

            void LoadMeshes(const aiScene* aScene);
            void LoadAnimations(const aiScene* aScene);

            // Skin vertices on CPU.
            void MeshTransform(const std::vector<glm::mat4>& transforms);

            std::vector<VertexType::SkinVertex> vertices;
            std::vector<unsigned int> indices;
            std::vector<glm::vec3*> verticesPos;
    };
}
