#pragma once

#include "VulkanDeviceObject.h"

namespace liquid::rhi {

/**
 * @brief Vulkan frame manager
 *
 * Stores synchronization primitives
 * for all frames in flight and advances
 * the frame
 */
class VulkanFrameManager {
public:
  /**
   * @brief Number of frames
   */
  static constexpr uint32_t NUM_FRAMES = 2;

public:
  /**
   * @brief Create frame manager
   *
   * @param device Vulkan device
   */
  VulkanFrameManager(VulkanDeviceObject &device);

  /**
   * @brief Destroy frame manager
   */
  ~VulkanFrameManager();

  VulkanFrameManager(const VulkanFrameManager &) = delete;
  VulkanFrameManager &operator=(const VulkanFrameManager &) = delete;
  VulkanFrameManager(VulkanFrameManager &&) = delete;
  VulkanFrameManager &operator=(VulkanFrameManager &&) = delete;

  /**
   * @brief Get frame fence
   *
   * @return Frame fence
   */
  inline VkFence getFrameFence() const { return mFrameFences.at(mFrameIndex); }

  /**
   * @brief Get image available semaphore
   *
   * @return Image available semaphore
   */
  inline VkSemaphore getImageAvailableSemaphore() const {
    return mImageAvailableSemaphores.at(mFrameIndex);
  }

  /**
   * @brief Get render finished semaphore
   *
   * @return Render finished semaphore
   */
  inline VkSemaphore getRenderFinishedSemaphore() const {
    return mRenderFinishedSemaphores.at(mFrameIndex);
  }

  /**
   * @brief Get current frame index
   *
   * @return Current frame index
   */
  inline uint32_t getCurrentFrameIndex() const { return mFrameIndex; }

  /**
   * @brief Go to next frame
   */
  void nextFrame();

  /**
   * @brief Wait for frame
   */
  void waitForFrame();

private:
  /**
   * @brief Create semaphores
   */
  void createSemaphores();

  /**
   * @brief Create fences
   */
  void createFences();

private:
  VulkanDeviceObject &mDevice;

  std::array<VkFence, NUM_FRAMES> mFrameFences{};
  std::array<VkSemaphore, NUM_FRAMES> mImageAvailableSemaphores{};
  std::array<VkSemaphore, NUM_FRAMES> mRenderFinishedSemaphores{};

  uint32_t mFrameIndex = 0;
};

} // namespace liquid::rhi
