#pragma once

#include "liquid/rhi/RenderDevice.h"
#include "liquid/rhi/ResourceRegistry.h"
#include "liquid/rhi/DeviceStats.h"

#include "VulkanPhysicalDevice.h"
#include "VulkanDeviceObject.h"
#include "VulkanQueue.h"
#include "VulkanRenderContext.h"
#include "VulkanUploadContext.h"
#include "VulkanRenderBackend.h"
#include "VulkanResourceAllocator.h"
#include "VulkanResourceRegistry.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorManager.h"
#include "VulkanSwapchain.h"

namespace liquid::rhi {

class VulkanRenderDevice : public RenderDevice {
public:
  /**
   * @brief Create Vulkan render device
   *
   * @param backend Render backend
   * @param physicalDevice Physical device
   * @param framebufferSize Framebuffer size
   */
  VulkanRenderDevice(VulkanRenderBackend &backend,
                     const VulkanPhysicalDevice &physicalDevice);

  /**
   * @brief Execute render graph
   *
   * @param graph Render graph
   * @param evaluator Render graph evaluator
   */
  void execute(RenderGraph &graph, RenderGraphEvaluator &evaluator) override;

  /**
   * @brief Wait for idle
   */
  void waitForIdle() override;

  /**
   * @brief Get physical device information
   *
   * @return Physical device information
   */
  const PhysicalDeviceInformation getDeviceInformation() override {
    return mPhysicalDevice.getDeviceInfo();
  }

  /**
   * @brief Get device stats
   *
   * @return Device stats
   */
  const DeviceStats &getDeviceStats() const override { return mStats; }

private:
  /**
   * @brief Recreate swapchain
   */
  void recreateSwapchain();

  /**
   * @brief Synchronize swapchain images
   *
   * @param prevNumSwapchainImages Previous swapchain image count
   */
  void synchronizeSwapchain(size_t prevNumSwapchainImages);

  /**
   * @brief Synchronize resources
   *
   * @param registry Resource registry
   */
  void synchronize(ResourceRegistry &registry);

private:
  DeviceStats mStats;

  VulkanRenderBackend &mBackend;
  VulkanPhysicalDevice mPhysicalDevice;
  VulkanDeviceObject mDevice;
  VulkanQueue mPresentQueue;
  VulkanQueue mGraphicsQueue;

  VulkanResourceAllocator mAllocator;
  VulkanResourceRegistry mRegistry;
  VulkanDescriptorManager mDescriptorManager;
  VulkanCommandPool mCommandPool;
  VulkanRenderContext mRenderContext;
  VulkanUploadContext mUploadContext;
  VulkanSwapchain mSwapchain;

  bool mSwapchainRecreated = false;
};

} // namespace liquid::rhi
