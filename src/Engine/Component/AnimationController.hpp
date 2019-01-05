#pragma once

#include "SuperComponent.hpp"
#include <string>
#include <glm/glm.hpp>

namespace Animation {
class AnimationAction;
class AnimationTransition;
class AnimationController;
class Skeleton;
} // namespace Animation

namespace Component {
/// Animation controller.
class AnimationController : public SuperComponent {
  public:
    /// Create new animation controller component.
    AnimationController();

    Json::Value Save() const override;

    /// Update the animation controller.
    /**
     * @param deltaTime Time between frames.
     */
    void UpdateAnimation(float deltaTime);

    /// Set a bool in the state machine.
    /**
     * @param name Name of the bool.
     * @param value Value to set.
     */
    void SetBool(const std::string& name, bool value);

    /// Set a float in the state machine.
    /**
     * @param name Name of the float.
     * @param value Value to set.
     */
    void SetFloat(const std::string& name, float value);

    /// Get a bool in the state machine.
    /**
     * @param name Name of the bool.
     * @return Value of the bool.
     */
    bool GetBool(const std::string& name);

    /// Get a float in the state machine.
    /**
     * @param name Name of the float.
     * @return Value of the float.
     */
    float GetFloat(const std::string& name);

    /// Vector with the final calculated bones.
    std::vector<glm::mat4> bones;

    /// The controller.
    Animation::AnimationController* controller = nullptr;

    /// The skeleton.
    Animation::Skeleton* skeleton = nullptr;

  private:
    void Animate(float deltaTime, Animation::AnimationAction* action, unsigned int skeletonId = 0);
    void Blend(float deltaTime);

    Animation::AnimationAction* activeAction1 = nullptr;
    Animation::AnimationAction* activeAction2 = nullptr;
    Animation::AnimationTransition* activeTransition = nullptr;

    std::vector<glm::mat4> bonesToInterpolate1;
    std::vector<glm::mat4> bonesToInterpolate2;

    glm::vec3 position1 = glm::vec3(0.f);
    glm::vec3 position2 = glm::vec3(0.f);

    bool isBlending = false;
};
} // namespace Component
