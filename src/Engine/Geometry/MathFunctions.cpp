#include "MathFunctions.hpp"
#include <glm/gtc/quaternion.hpp>

void Geometry::MixVec(const glm::vec3& v1, const glm::vec3& v2, float t, glm::vec3& result) {
    result.x = v1.x + t * (v2.x - v1.x);
    result.y = v1.y + t * (v2.y - v1.y);
    result.z = v1.z + t * (v2.z - v1.z);
}

void Geometry::MixQuat(const glm::quat& q1, const glm::quat& q2, float t, glm::quat& result) {
    glm::quat tmp = q1;
    if (DotQuat(q1, q2) < 0) {
        tmp.x = -q1.x;
        tmp.y = -q1.y;
        tmp.z = -q1.z;
        tmp.w = -q1.w;
    }
    result.x = tmp.x + t * (q2.x - tmp.x);
    result.y = tmp.y + t * (q2.y - tmp.y);
    result.z = tmp.z + t * (q2.z - tmp.z);
    result.w = tmp.w + t * (q2.w - tmp.w);
    NormalizeQuat(result);
}

float Geometry::DotQuat(const glm::quat& q1, const glm::quat& q2) {
    return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
}

void Geometry::NormalizeQuat(glm::quat& q) {
    float d = sqrt(DotQuat(q, q));
    if (d >= 0.00001) {
        d = 1 / d;
        q.x *= d;
        q.y *= d;
        q.z *= d;
        q.w *= d;
    }
    else {
        q.x = q.y = q.z = 0;
        q.w = 1;
    }
}

void Geometry::ComposeMatrix(const glm::vec3& p, glm::quat& r, const glm::vec3& s, glm::mat4& m) {
    // Rotation
    m[0][0] = 1.f - 2.f * (r.y * r.y + r.z * r.z);
    m[0][1] = 2.f * (r.x * r.y - r.z * r.w);
    m[0][2] = 2.f * (r.x * r.z + r.y * r.w);
    m[1][0] = 2.f * (r.x * r.y + r.z * r.w);
    m[1][1] = 1.f - 2.f * (r.x * r.x + r.z * r.z);
    m[1][2] = 2.f * (r.y * r.z - r.x * r.w);
    m[2][0] = 2.f * (r.x * r.z - r.y * r.w);
    m[2][1] = 2.f * (r.y * r.z + r.x * r.w);
    m[2][2] = 1.f - 2.f * (r.x * r.x + r.y * r.y);

    // Scale
    m[0][0] *= s.x;
    m[0][1] *= s.x;
    m[0][2] *= s.x;
    m[1][0] *= s.y;
    m[1][1] *= s.y;
    m[1][2] *= s.y;
    m[2][0] *= s.z;
    m[2][1] *= s.z;
    m[2][2] *= s.z;

    // Translation
    m[0][3] = p.x;
    m[1][3] = p.y;
    m[2][3] = p.z;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
}

void Geometry::QuatToMat(glm::mat4& m, const glm::quat& q) {
    glm::vec4 r(q.w, q.x, q.y, q.z);
    m[0][0] = 1.f - 2.f * (r.y * r.y + r.z * r.z);
    m[0][1] = 2.f * (r.x * r.y - r.z * r.w);
    m[0][2] = 2.f * (r.x * r.z + r.y * r.w);
    m[1][0] = 2.f * (r.x * r.y + r.z * r.w);
    m[1][1] = 1.f - 2.f * (r.x * r.x + r.z * r.z);
    m[1][2] = 2.f * (r.y * r.z - r.x * r.w);
    m[2][0] = 2.f * (r.x * r.z - r.y * r.w);
    m[2][1] = 2.f * (r.y * r.z + r.x * r.w);
    m[2][2] = 1.f - 2.f * (r.x * r.x + r.y * r.y);
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

void Geometry::CpyQuat(glm::quat &glmQuat, const aiQuaternion& aiQuat) {
    glmQuat.x = aiQuat.x;
    glmQuat.y = aiQuat.y;
    glmQuat.z = aiQuat.z;
    glmQuat.w = aiQuat.w;
}
