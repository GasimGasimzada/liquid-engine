#pragma once

#include "liquid/core/Property.h"

namespace liquid {

enum class PhysicalDeviceType {
  DISCRETE_GPU,
  INTEGRATED_GPU,
  VIRTUAL_GPU,
  CPU,
  UNKNOWN
};

class PhysicalDeviceInformation {
public:
  using UnorderedPropertyMap = std::vector<std::pair<String, Property>>;

public:
  /**
   * @brief Create physical device information
   *
   * @param name Device name
   * @param type Device type
   * @param properties Device properties
   * @param limits Device limits
   */
  PhysicalDeviceInformation(const String &name, PhysicalDeviceType type,
                            const UnorderedPropertyMap &properties,
                            const UnorderedPropertyMap &limits);

  /**
   * @brief Get device type
   *
   * @return Physical device type
   */
  inline const PhysicalDeviceType getType() const { return mType; }

  /**
   * @brief Get device name
   *
   * @return Physical device name
   */
  inline const String &getName() const { return mName; }

  /**
   * @brief Get device properties
   *
   * @return Device properties
   */
  inline const UnorderedPropertyMap &getProperties() const {
    return mProperties;
  }

  /**
   * @brief Get device limits
   *
   * @return Device limits
   */
  inline const UnorderedPropertyMap &getLimits() const { return mLimits; }

  /**
   * @brief Get type string proeprties
   *
   * @return Type string
   */
  const String getTypeString() const;

private:
  String mName;
  PhysicalDeviceType mType;

  UnorderedPropertyMap mProperties;
  UnorderedPropertyMap mLimits;
};

} // namespace liquid
