#pragma once

#include <assimp/vector2.h>
#include <assimp/vector3.h>
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <glm/glm.hpp>

namespace Geometry {
    /// Math functions between assimp and glm.
    void MixVec(const glm::vec3& v1, const glm::vec3& v2, float t, glm::vec3& result);
    void MixQuat(const glm::quat& q1, const glm::quat& q2, float t, glm::quat& result);
    float DotQuat(const glm::quat& q1, const glm::quat& q2);
    void NormalizeQuat(glm::quat& q);
    void ComposeMatrix(const glm::vec3& p, glm::quat& r, const glm::vec3& s, glm::mat4& matrix);

    /// Assimp math to glm math.
    void QuatToMat(glm::mat4& m, const glm::quat& q);
    void CpyQuat(glm::quat &glmQuat, const aiQuaternion& aiQuat);
    void CpyVec(glm::vec3& glmVec, const aiVector3D& aiVec);
    void CpyVec(glm::vec2& glmVec, const aiVector3D& aiVec);
    void CpyVec(glm::vec2& glmVec, const aiVector2D& aiVec);
    void CpyMat(glm::mat4& glmMat, const aiMatrix4x4& aiMat);
    void CpyMat(aiMatrix3x3& aiMat3, const aiMatrix4x4& aiMat4);
}
