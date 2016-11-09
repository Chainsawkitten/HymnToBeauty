#pragma once

#include "Geometry3D.hpp"
#include <assimp/Importer.hpp>
#include <json/json.h>

namespace Geometry {
    /// Interface of a model loaded from a file.
    class Model : public Geometry3D {
        public:
            /// Create new empty model.
            /**
             * The created model has to be loaded later using Load.
             */
            Model();

            /// Destructor.
            virtual ~Model();

            /// Save the model.
            /**
             * @return JSON value to be stored on disk.
             */
            Json::Value Save() const;

            /// Load model from JSON node.
            /**
             * @param node JSON node to load from.
             */
            void Load(const Json::Value& node);

            /// Load model from file.
            /**
             * @param filename Filename (relative or absolute) to the model file.
             */
            virtual void Load(const char* filename) = 0;

            /// Get geometry type.
            /**
             * @return Type.
             */
            virtual Type GetType() const = 0;

            /// The name of the model.
            std::string name;

            /// The extension of the model.
            std::string extension;

        protected:
            /// Generate vertex buffer.
            /**
             * @param vertexBuffer Vertex buffer.
             */
            virtual void GenerateVertexBuffer(GLuint& vertexBuffer) = 0;

            /// Generate vertex array.
            /**
             * @param vertexBuffer Vertex buffer.
             * @param indexBuffer Index buffer.
             * @param vertexArray Vertex array.
             */
            virtual void GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray) = 0;

            /// Assimp importer.
            static Assimp::Importer aImporter;
    };
}
