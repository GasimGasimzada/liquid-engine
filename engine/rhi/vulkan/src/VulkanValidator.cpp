#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"

#include "VulkanValidator.h"
#include "VulkanError.h"

namespace liquid::rhi {

VulkanValidator::VulkanValidator() {
  mMessengerCreateInfo.sType =
      VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  mMessengerCreateInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  mMessengerCreateInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  mMessengerCreateInfo.pfnUserCallback = VulkanValidator::debugCallback;
  mMessengerCreateInfo.pUserData = nullptr;
  mMessengerCreateInfo.flags = 0;
  mMessengerCreateInfo.pNext = nullptr;
}

void VulkanValidator::detachFromInstance(VkInstance instance) {
  if (mMessenger) {
    VulkanValidator::destroyDebugUtilsMessengerEXT(instance, mMessenger,
                                                   nullptr);
  }
}

void VulkanValidator::attachToInstanceCreateConfig(
    VkInstanceCreateInfo &createInfo) {
  createInfo.pNext =
      (VkDebugUtilsMessengerCreateInfoEXT *)&mMessengerCreateInfo;
  createInfo.ppEnabledLayerNames = mValidationLayers.data();
  createInfo.enabledLayerCount =
      static_cast<uint32_t>(mValidationLayers.size());
}

void VulkanValidator::attachToInstance(VkInstance instance) {
  LIQUID_ASSERT(checkValidationSupport(), "Cannot create debug messenger");

  checkForVulkanError(
      VulkanValidator::createDebugUtilsMessengerEXT(
          instance, &mMessengerCreateInfo, nullptr, &mMessenger),
      "Cannot create debug messenger");
}

VkResult VulkanValidator::createDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  }

  return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void VulkanValidator::destroyDebugUtilsMessengerEXT(
    VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks *pAllocator) {
  auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
      vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanValidator::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    engineLogger.log(Logger::Warning) << "[Vulkan] " << pCallbackData->pMessage;
  }

  return VK_FALSE;
}

bool VulkanValidator::checkValidationSupport() {
  uint32_t layerCount = 0;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (auto &neededLayer : mValidationLayers) {
    auto it = std::find_if(availableLayers.begin(), availableLayers.end(),
                           [neededLayer](auto &layer) {
                             return strcmp((const char *)layer.layerName,
                                           (const char *)neededLayer) == 0;
                           });

    if (it == availableLayers.end()) {
      return false;
    }
  }

  return true;
}

} // namespace liquid::rhi
