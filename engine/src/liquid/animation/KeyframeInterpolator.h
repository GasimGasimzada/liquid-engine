#pragma once

#include "liquid/animation/AnimationAsset.h"

namespace liquid {

/**
 * @brief Interpolates keyframes
 */
class KeyframeInterpolator {
public:
  /**
   * @brief Get interpolated value based on keyframe sequence
   *
   * @param sequence Keyframe sequence
   * @param time Time
   * @return Interpolated value
   */
  const glm::vec4 interpolate(const KeyframeSequenceAsset &sequence,
                              float time) const;

private:
  /**
   * @brief Get step interpolated value
   *
   * Gets the previous interpolated value
   *
   * @param sequence Keyframe sequence
   * @param time Time
   * @return Value at previous time
   */
  const glm::vec4 &
  getStepInterpolatedValue(const KeyframeSequenceAsset &sequence,
                           float time) const;

  /**
   * @brief Get linear interpolated value between two times
   *
   * @param sequence Keyframe sequence
   * @param time Time
   * @return Interpolated value
   */
  const glm::vec4
  getLinearInterpolatedValue(const KeyframeSequenceAsset &sequence,
                             float time) const;
};

} // namespace liquid
