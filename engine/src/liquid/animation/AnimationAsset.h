#pragma once

#include "liquid/scene/Joint.h"

namespace liquid {

enum class KeyframeSequenceAssetTarget : uint8_t { Position, Rotation, Scale };

enum class KeyframeSequenceAssetInterpolation : uint8_t { Step, Linear };

/**
 * @brief Keyframe sequence asset
 */
struct KeyframeSequenceAsset {
  /**
   * List of keyframe times
   */
  std::vector<float> keyframeTimes;

  /**
   * List of keyframe values
   */
  std::vector<glm::vec4> keyframeValues;

  /**
   * Joint ID
   */
  JointId joint = 0;

  /**
   * Is joint target
   */
  bool jointTarget = false;

  /**
   * Transform target
   */
  KeyframeSequenceAssetTarget target = KeyframeSequenceAssetTarget::Position;

  /**
   * Keyframe interpolation
   */
  KeyframeSequenceAssetInterpolation interpolation =
      KeyframeSequenceAssetInterpolation::Step;
};

/**
 * Animation asset data
 */
struct AnimationAsset {
  /**
   * Animation time
   */
  float time = 0.0f;

  /**
   * Keyframes
   */
  std::vector<KeyframeSequenceAsset> keyframes;
};

} // namespace liquid
