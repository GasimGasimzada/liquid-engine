#pragma once

#include "liquid/scene/Joint.h"

namespace liquid {

/**
 * @brief Skeleton asset data
 */
struct SkeletonAsset {
  /**
   * Joint parents
   */
  std::vector<JointId> jointParents;

  /**
   * Joint local positions
   */
  std::vector<glm::vec3> jointLocalPositions;

  /**
   * Joint local rotations
   */
  std::vector<glm::quat> jointLocalRotations;

  /**
   * Joint local scales
   */
  std::vector<glm::vec3> jointLocalScales;

  /**
   * Joint inverse bind matrices
   */
  std::vector<glm::mat4> jointInverseBindMatrices;

  /**
   * Joint names
   */
  std::vector<String> jointNames;
};

} // namespace liquid
