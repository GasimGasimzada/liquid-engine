#pragma once

#include "liquid/rhi/RenderCommandList.h"
#include "liquid/rhi/DeviceStats.h"

#include "VulkanDeviceObject.h"
#include "VulkanDescriptorManager.h"

namespace liquid::rhi {

/**
 * @brief Vulkan command pool
 */
class VulkanCommandPool {
public:
  /**
   * @brief Create command pool
   *
   * @param device Render device
   * @param queueFamilyIndex Queue family index
   * @param registry Vulkan registry
   * @param descriptorManager Descriptor manager
   * @param stats Device stats
   */
  VulkanCommandPool(VulkanDeviceObject &device, uint32_t queueFamilyIndex,
                    const VulkanResourceRegistry &registry,
                    VulkanDescriptorManager &descriptorManager,
                    DeviceStats &stats);

  /**
   * @brief Destroy command pool
   */
  ~VulkanCommandPool();

  VulkanCommandPool(const VulkanCommandPool &) = delete;
  VulkanCommandPool &operator=(const VulkanCommandPool &) = delete;
  VulkanCommandPool(VulkanCommandPool &&) = delete;
  VulkanCommandPool &operator=(VulkanCommandPool &&) = delete;

  /**
   * @brief Create command buffers
   *
   * @param count Number of buffers
   * @return List of render command lists
   */
  std::vector<RenderCommandList> createCommandLists(uint32_t count);

private:
  VkCommandPool mCommandPool = VK_NULL_HANDLE;
  VulkanDeviceObject &mDevice;
  VulkanDescriptorManager &mDescriptorManager;
  DeviceStats &mStats;
  const VulkanResourceRegistry &mRegistry;
};

} // namespace liquid::rhi
