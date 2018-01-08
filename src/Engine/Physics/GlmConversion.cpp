#include "GlmConversion.hpp"

namespace Physics {
    btVector3 glmToBt(glm::vec3 const& vec) {
        return btVector3(vec.x, vec.y, vec.z);
    }

    glm::vec3 btToGlm(btVector3 const& vec) {
        return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
    }

    btQuaternion glmToBt(const glm::quat& quat) {
        return btQuaternion(quat.x, quat.y, quat.z, quat.w);
    }

    glm::quat btToGlm(btQuaternion const& quat) {
        return glm::quat(quat.getW(), quat.getX(), quat.getY(), quat.getZ());
    }
}
