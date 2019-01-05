#include "MathFunctions.hpp"
#include <glm/gtc/quaternion.hpp>

float Geometry::DotQuat(const glm::quat& q1, const glm::quat& q2) {
    return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

void Geometry::NormalizeQuat(glm::quat& q) {
    float d = sqrt(DotQuat(q, q));
    if (d >= 0.00001) {
        d = 1 / d;
        q.x *= d;
        q.y *= d;
        q.z *= d;
        q.w *= d;
    } else {
        q.x = q.y = q.z = 0;
        q.w = 1;
    }
}

void Geometry::CpyVec(glm::vec3& glmVec, const aiVector3D& aiVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
    glmVec.z = aiVec.z;
}

void Geometry::CpyVec(glm::vec2& glmVec, const aiVector3D& aiVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
}

void Geometry::CpyVec(glm::vec2& glmVec, const aiVector2D& aiVec) {
    glmVec.x = aiVec.x;
    glmVec.y = aiVec.y;
}

void Geometry::CpyMat(glm::mat4& glmMat, const aiMatrix4x4& aiMat) {
    glmMat[0][0] = aiMat.a1;
    glmMat[0][1] = aiMat.a2;
    glmMat[0][2] = aiMat.a3;
    glmMat[0][3] = aiMat.a4;

    glmMat[1][0] = aiMat.b1;
    glmMat[1][1] = aiMat.b2;
    glmMat[1][2] = aiMat.b3;
    glmMat[1][3] = aiMat.b4;

    glmMat[2][0] = aiMat.c1;
    glmMat[2][1] = aiMat.c2;
    glmMat[2][2] = aiMat.c3;
    glmMat[2][3] = aiMat.c4;

    glmMat[3][0] = aiMat.d1;
    glmMat[3][1] = aiMat.d2;
    glmMat[3][2] = aiMat.d3;
    glmMat[3][3] = aiMat.d4;
}

void Geometry::CpyMat(aiMatrix3x3& aiMat3, const aiMatrix4x4& aiMat4) {
    aiMat3.a1 = aiMat4.a1;
    aiMat3.a2 = aiMat4.a2;
    aiMat3.a3 = aiMat4.a3;

    aiMat3.b1 = aiMat4.b1;
    aiMat3.b2 = aiMat4.b2;
    aiMat3.b3 = aiMat4.b3;

    aiMat3.c1 = aiMat4.c1;
    aiMat3.c2 = aiMat4.c2;
    aiMat3.c3 = aiMat4.c3;
}
