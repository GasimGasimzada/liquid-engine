#include "liquid/core/Base.h"

#include <vulkan/vulkan.hpp>
#include "VulkanRenderDevice.h"
#include "liquid/renderer/vulkan/VulkanError.h"

#include "liquid/core/EngineGlobals.h"

namespace liquid::experimental {

const String LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME =
    "VK_KHR_portability_subset";

VulkanRenderDevice::VulkanRenderDevice(
    const VulkanPhysicalDevice &physicalDevice, VulkanRenderBackend &backend)
    : mPhysicalDevice(physicalDevice), mBackend(backend) {
  createVulkanDevice();
  getDeviceQueues();
  createResourceManager();
}

VulkanRenderDevice::~VulkanRenderDevice() {
  for (const auto &[_, x] : mRegistry.getBuffers()) {
    mManager.destroyBuffer(x);
  }

  for (const auto &[_, x] : mRegistry.getTextures()) {
    mManager.destroyTexture(x);
  }

  mManager.destroy();

  if (mDevice) {
    vkDestroyDevice(mDevice, nullptr);
    LOG_DEBUG("[Vulkan] Device for " << mPhysicalDevice.getName()
                                     << " destroyed");
  }
}

void VulkanRenderDevice::synchronize(ResourceRegistry &registry) {
  for (auto &handle : registry.getBufferMap().getDirtyCreates()) {
    auto &&buffer =
        mManager.createBuffer(registry.getBufferMap().getDescription(handle));
    mRegistry.addBuffer(handle, std::move(buffer));
  }

  registry.getBufferMap().clearDirtyCreates();

  for (auto &handle : registry.getBufferMap().getDirtyUpdates()) {
    if (registry.getBufferMap().hasDescription(handle)) {
      auto &buffer = mRegistry.getBuffer(handle);
      auto newBuffer = mManager.updateBuffer(
          buffer, registry.getBufferMap().getDescription(handle));
      if (newBuffer) {
        mRegistry.updateBuffer(handle, std::move(newBuffer));
      }
    }
  }
  registry.getBufferMap().clearDirtyUpdates();

  for (auto &handle : registry.getTextureMap().getDirtyCreates()) {
    if (registry.getTextureMap().hasDescription(handle)) {
      auto &&texture = mManager.createTexture(
          registry.getTextureMap().getDescription(handle));
      mRegistry.addTexture(handle, std::move(texture));
    }
  }
  registry.getTextureMap().clearDirtyCreates();
}

void VulkanRenderDevice::synchronizeDeletes(ResourceRegistry &registry) {
  for (auto &handle : registry.getBufferMap().getDirtyDeletes()) {
    mManager.destroyBuffer(mRegistry.getBuffer(handle));
    mRegistry.removeBuffer(handle);
  }

  registry.getBufferMap().clearDirtyDeletes();

  for (auto &handle : registry.getTextureMap().getDirtyDeletes()) {
    mManager.destroyTexture(mRegistry.getTexture(handle));
    mRegistry.removeTexture(handle);
  }

  registry.getBufferMap().clearDirtyDeletes();
}

void VulkanRenderDevice::createVulkanDevice() {
  float queuePriority = 1.0f;

  VkDeviceQueueCreateInfo createGraphicsQueueInfo{};
  createGraphicsQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  createGraphicsQueueInfo.flags = 0;
  createGraphicsQueueInfo.pNext = nullptr;
  createGraphicsQueueInfo.queueFamilyIndex =
      mPhysicalDevice.getQueueFamilyIndices().graphicsFamily.value();
  createGraphicsQueueInfo.queueCount = 1;
  createGraphicsQueueInfo.pQueuePriorities = &queuePriority;

  VkDeviceQueueCreateInfo createPresentQueueInfo{};
  createPresentQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  createPresentQueueInfo.flags = 0;
  createPresentQueueInfo.pNext = nullptr;
  createPresentQueueInfo.queueFamilyIndex =
      mPhysicalDevice.getQueueFamilyIndices().presentFamily.value();
  createPresentQueueInfo.queueCount = 1;
  createPresentQueueInfo.pQueuePriorities = &queuePriority;

  std::vector<VkDeviceQueueCreateInfo> queueInfos;
  queueInfos.push_back(createGraphicsQueueInfo);
  if (mPhysicalDevice.getQueueFamilyIndices().graphicsFamily.value() !=
      mPhysicalDevice.getQueueFamilyIndices().presentFamily.value()) {
  }

  const auto &pdExtensions = mPhysicalDevice.getSupportedExtensions();

  std::vector<const char *> extensions;
  extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  LOG_DEBUG("[Vulkan] Extension enabled: " << VK_KHR_SWAPCHAIN_EXTENSION_NAME);

  extensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
  LOG_DEBUG("[Vulkan] Extension enabled: "
            << VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

  extensions.push_back(VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);
  LOG_DEBUG("[Vulkan] Extension enabled: "
            << VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME);

  const auto &portabilityExt = std::find_if(
      pdExtensions.cbegin(), pdExtensions.cend(), [](const auto &ext) {
        return String(static_cast<const char *>(ext.extensionName)) ==
               LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME;
      });

  if (portabilityExt != pdExtensions.end()) {
    extensions.push_back(
        LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME.c_str());
    LOG_DEBUG("[Vulkan] Extension enabled: "
              << LIQUID_VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
  }

  VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures{};
  descriptorIndexingFeatures.sType =
      VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
  descriptorIndexingFeatures.pNext = nullptr;
  descriptorIndexingFeatures.descriptorBindingPartiallyBound = true;

  auto &features = mPhysicalDevice.getFeatures();

  VkDeviceCreateInfo createDeviceInfo{};
  createDeviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createDeviceInfo.pNext = &descriptorIndexingFeatures;
  createDeviceInfo.flags = 0;

  createDeviceInfo.pQueueCreateInfos = queueInfos.data();
  createDeviceInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueInfos.size());
  createDeviceInfo.pEnabledFeatures = &features;
  createDeviceInfo.enabledLayerCount = 0;
  createDeviceInfo.ppEnabledLayerNames = nullptr;
  createDeviceInfo.enabledExtensionCount =
      static_cast<uint32_t>(extensions.size());
  createDeviceInfo.ppEnabledExtensionNames = extensions.data();

  checkForVulkanError(vkCreateDevice(mPhysicalDevice.getVulkanDevice(),
                                     &createDeviceInfo, nullptr, &mDevice),
                      "Failed to create device");

  LOG_DEBUG("[Vulkan] Vulkan device created for " << mPhysicalDevice.getName());
}

void VulkanRenderDevice::createResourceManager() { mManager.create(this); }

void VulkanRenderDevice::getDeviceQueues() {
  vkGetDeviceQueue(
      mDevice, mPhysicalDevice.getQueueFamilyIndices().presentFamily.value(), 0,
      &mPresentQueue);

  vkGetDeviceQueue(
      mDevice, mPhysicalDevice.getQueueFamilyIndices().graphicsFamily.value(),
      0, &mGraphicsQueue);
}

} // namespace liquid::experimental