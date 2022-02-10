#include "liquid/core/Base.h"
#include "AnimationSystem.h"

namespace liquid {

AnimationSystem::AnimationSystem(EntityContext &entityContext_)
    : entityContext(entityContext_) {}

void AnimationSystem::addAnimation(const Animation &animation) {
  animations.insert({animation.getName(), animation});
}

void AnimationSystem::update(float dt) {
  LIQUID_PROFILE_EVENT("AnimationSystem::update");
  entityContext.iterateEntities<TransformComponent, AnimatorComponent>(
      [this, dt](Entity entity, auto &transform, auto &animComp) {
        const auto &iter = animations.find(animComp.animation);
        if (iter == animations.end()) {
          return;
        }

        const Animation &animation = (*iter).second;

        if (animComp.playing) {
          animComp.currentTime =
              std::min(animComp.currentTime + dt, animation.getTime());
          if (animComp.loop && animComp.currentTime >= animation.getTime()) {
            animComp.currentTime = 0.0f;
          }
        }

        float normalizedTime = animComp.currentTime / animation.getTime();

        bool hasSkeleton =
            entityContext.hasComponent<SkeletonComponent>(entity);

        for (auto &sequence : animation.getKeyframeSequences()) {
          const auto &value = sequence.getInterpolatedValue(normalizedTime);

          if (sequence.isJointTarget() && hasSkeleton) {
            auto &skeleton =
                entityContext.getComponent<SkeletonComponent>(entity).skeleton;
            if (sequence.getTarget() == KeyframeSequenceTarget::Position) {
              skeleton.setJointPosition(sequence.getJoint(), glm::vec3(value));
            } else if (sequence.getTarget() ==
                       KeyframeSequenceTarget::Rotation) {
              skeleton.setJointRotation(
                  sequence.getJoint(),
                  glm::quat(value.w, value.x, value.y, value.z));
            } else if (sequence.getTarget() == KeyframeSequenceTarget::Scale) {
              skeleton.setJointScale(sequence.getJoint(), glm::vec3(value));
            }
            skeleton.update();
          } else {
            if (sequence.getTarget() == KeyframeSequenceTarget::Position) {
              transform.localPosition = glm::vec3(value);
            } else if (sequence.getTarget() ==
                       KeyframeSequenceTarget::Rotation) {
              transform.localRotation =
                  glm::quat(value.w, value.x, value.y, value.z);
            } else if (sequence.getTarget() == KeyframeSequenceTarget::Scale) {
              transform.localScale = glm::vec3(value);
            }
          }
        }
      });
}

} // namespace liquid