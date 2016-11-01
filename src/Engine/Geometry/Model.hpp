#pragma once

#include "Geometry3D.hpp"
#include <assimp/Importer.hpp>

namespace Geometry {
    /// Interface of a model loaded from an file.
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
        virtual const Type GetType() const = 0;

        /// The name of the model.
        std::string name;

        /// The extension of the model.
        std::string extension;

    protected:
        /// Generate vertex buffer.
        virtual void GenerateVertexBuffer(GLuint& vertexBuffer) = 0;

        /// Generate vertex array.
        virtual void GenerateVertexArray(const GLuint vertexBuffer, const GLuint indexBuffer, GLuint& vertexArray) = 0;

        /// Math helper fucntions.
        static void MixVec(const glm::vec3& v1, const glm::vec3& v2, float t, glm::vec3& result);
        static void MixQuat(const glm::quat& q1, const glm::quat& q2, float t, glm::quat& result);
        static float DotQuat(const glm::quat& q1, const glm::quat& q2);
        static void NormalizeQuat(glm::quat& q);
        static void ComposeMatrix(const glm::vec3& p, glm::quat& r, const glm::vec3& s, glm::mat4& matrix);

        /// Assimp math to glm math.
        static void QuatToMat(glm::mat4& m, const glm::quat& q);
        static void CpyQuat(glm::quat &glmQuat, const aiQuaternion& aiQuat);
        static void CpyVec(glm::vec3& glmVec, const aiVector3D& aiVec);
        static void CpyVec(glm::vec2& glmVec, const aiVector3D& aiVec);
        static void CpyVec(glm::vec2& glmVec, const aiVector2D& aiVec);
        static void CpyMat(glm::mat4& glmMat, const aiMatrix4x4& aiMat);
        static void CpyMat(aiMatrix3x3& aiMat3, const aiMatrix4x4& aiMat4);

        static Assimp::Importer aImporter;
    };
}
