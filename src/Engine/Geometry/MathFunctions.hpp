#pragma once

#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/glm.hpp>

namespace Geometry {
    /// Mix two vectors.
    /**
     * @param v1 First vector.
     * @param v2 Second vector.
     * @param t Mix factor.
     * @param result Mixed vector.
     */
    void MixVec(const glm::vec3& v1, const glm::vec3& v2, float t, glm::vec3& result);

    /// Mix two quaternions.
    /**
     * @param q1 First quaternion.
     * @param q2 Second quaternion.
     * @param t Mix factor.
     * @param result Mixed quaternion.
     */
    void MixQuat(const glm::quat& q1, const glm::quat& q2, float t, glm::quat& result);

    /// Calculate the dot product of two quaternions.
    /**
     * @param q1 First quaternion.
     * @param q2 Second quaternion.
     * @return t Dot product value.
     */
    float DotQuat(const glm::quat& q1, const glm::quat& q2);

    /// Normalize quaternion.
    /**
     * @param q Quaternion to normalize.
     */
    void NormalizeQuat(glm::quat& q);

    /// Create matrix from position, rotation and scale.
    /**
     * @param p Position.
     * @param r Rotation.
     * @param s Scale.
     * @param matrix Transform matrix.
     */
    void ComposeMatrix(const glm::vec3& p, glm::quat& r, const glm::vec3& s, glm::mat4& matrix);

    /// Create matrix from quaternion.
    /**
     * @param m Matrix.
     * @param q Quaternion.
     */
    void QuatToMat(glm::mat4& m, const glm::quat& q);

    /// Convert from assimp quaternion to glm quaternion.
    /**
     * @param glmQuat Glm quaternion.
     * @param aiQuat Assimp quaternion.
     */
    void CpyQuat(glm::quat &glmQuat, const aiQuaternion& aiQuat);

    /// Convert from assimp vector3 to glm vector3.
    /**
     * @param glmVec Glm vector.
     * @param aiVec Assimp vector.
     */
    void CpyVec(glm::vec3& glmVec, const aiVector3D& aiVec);

    /// Convert from assimp vector3 to glm vector2.
    /**
     * @param glmVec Glm vector.
     * @param aiVec Assimp vector.
     */
    void CpyVec(glm::vec2& glmVec, const aiVector3D& aiVec);

    /// Convert from assimp vector2 to glm vector2.
    /**
     * @param glmVec Glm vector.
     * @param aiVec Assimp vector.
     */
    void CpyVec(glm::vec2& glmVec, const aiVector2D& aiVec);

    /// Convert from assimp matrix4x4 to glm matrix4x4.
    /**
     * @param glmMat Glm matrix.
     * @param aiMat Assimp matrix.
     */
    void CpyMat(glm::mat4& glmMat, const aiMatrix4x4& aiMat);

    /// Convert from assimp matrix4x4 to assimp matrix3x3.
    /**
     * @param aiMat3 Assimp matrix3x3.
     * @param aiMat4 Assimp matrix4x4.
     */
    void CpyMat(aiMatrix3x3& aiMat3, const aiMatrix4x4& aiMat4);
}
