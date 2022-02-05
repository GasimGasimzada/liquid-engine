#include "liquid/core/Base.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/renderer/RenderCommandList.h"
#include "VulkanResourceAllocator.h"
#include "VulkanError.h"
#include "VulkanHardwareBuffer.h"
#include "VulkanTextureBinder.h"

namespace liquid {

VulkanResourceAllocator *
VulkanResourceAllocator::create(const VulkanContext &vulkanInstance,
                                const VulkanUploadContext &uploadContext,
                                StatsManager &statsManager) {
  VmaAllocator allocator = nullptr;

  VmaAllocatorCreateInfo createInfo{};
  createInfo.instance = vulkanInstance.getInstance();
  createInfo.physicalDevice =
      vulkanInstance.getPhysicalDevice().getVulkanDevice();
  createInfo.device = vulkanInstance.getDevice();

  checkForVulkanError(vmaCreateAllocator(&createInfo, &allocator),
                      "Failed to create allocator");

  LOG_DEBUG("[Vulkan] Allocator created");

  return new VulkanResourceAllocator(uploadContext, allocator,
                                     vulkanInstance.getDevice(), statsManager);
}

VulkanResourceAllocator::VulkanResourceAllocator(
    const VulkanUploadContext &uploadContext_, VmaAllocator allocator_,
    VkDevice device_, StatsManager &statsManager_)
    : uploadContext(uploadContext_), allocator(allocator_), device(device_),
      statsManager(statsManager_) {}

VulkanResourceAllocator::~VulkanResourceAllocator() {
  if (allocator) {
    vmaDestroyAllocator(allocator);
    LOG_DEBUG("[Vulkan] Allocator destroyed");
  }
}

SharedPtr<HardwareBuffer>
VulkanResourceAllocator::createVertexBuffer(size_t bufferSize) {
  return createHardwareBuffer(HardwareBuffer::Vertex, bufferSize);
}

SharedPtr<HardwareBuffer>
VulkanResourceAllocator::createIndexBuffer(size_t bufferSize) {
  return createHardwareBuffer(HardwareBuffer::Index, bufferSize);
}

SharedPtr<HardwareBuffer>
VulkanResourceAllocator::createUniformBuffer(size_t bufferSize) {
  return createHardwareBuffer(HardwareBuffer::Uniform, bufferSize);
}

SharedPtr<VulkanHardwareBuffer> VulkanResourceAllocator::createHardwareBuffer(
    HardwareBuffer::HardwareBufferType bufferType, size_t bufferSize) {

  VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
  if (bufferType == HardwareBuffer::Vertex) {
    bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  } else if (bufferType == HardwareBuffer::Index) {
    bufferUsage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
  } else if (bufferType == HardwareBuffer::Uniform) {
    bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  } else if (bufferType == HardwareBuffer::Transfer) {
    bufferUsage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;
  }

  VkBufferCreateInfo createBufferInfo{};
  createBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  createBufferInfo.pNext = nullptr;
  createBufferInfo.flags = 0;
  createBufferInfo.size = bufferSize;
  createBufferInfo.usage = bufferUsage;

  VmaAllocationCreateInfo createAllocationInfo{};
  createAllocationInfo.flags = 0;
  createAllocationInfo.usage = memoryUsage;

  VmaAllocation allocation = nullptr;
  VkBuffer buffer = VK_NULL_HANDLE;

  checkForVulkanError(vmaCreateBuffer(allocator, &createBufferInfo,
                                      &createAllocationInfo, &buffer,
                                      &allocation, nullptr),
                      "Cannot create hardware buffer");
  return std::make_shared<VulkanHardwareBuffer>(
      bufferType, bufferSize, buffer, allocation, allocator, statsManager);
}

SharedPtr<Texture>
VulkanResourceAllocator::createTexture2D(const TextureData &textureData) {
  uint32_t textureSize = textureData.width * textureData.height * 4;

  auto hardwareBuffer =
      createHardwareBuffer(HardwareBuffer::Transfer, textureSize);
  VkBuffer buffer = hardwareBuffer->getBuffer();
  void *data = hardwareBuffer->map();
  memcpy(data, static_cast<void *>(textureData.data), textureSize);
  hardwareBuffer->unmap();

  VkExtent3D extent{};
  extent.width = textureData.width;
  extent.height = textureData.height;
  extent.depth = 1;

  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = 0;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
  imageCreateInfo.extent = extent;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.usage =
      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo imageAllocationInfo{};
  imageAllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  VmaAllocation allocation = VK_NULL_HANDLE;
  VkImage image = VK_NULL_HANDLE;
  VkImageView imageView = VK_NULL_HANDLE;
  VkSampler sampler = VK_NULL_HANDLE;

  checkForVulkanError(vmaCreateImage(allocator, &imageCreateInfo,
                                     &imageAllocationInfo, &image, &allocation,
                                     nullptr),
                      "Failed to create image");

  VkImageSubresourceRange range{};
  range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  range.baseMipLevel = 0;
  range.levelCount = 1;
  range.baseArrayLayer = 0;
  range.layerCount = 1;

  VkImageMemoryBarrier imageBarrierTransfer{};
  imageBarrierTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  imageBarrierTransfer.pNext = nullptr;
  imageBarrierTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageBarrierTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  imageBarrierTransfer.image = image;
  imageBarrierTransfer.subresourceRange = range;
  imageBarrierTransfer.srcAccessMask = 0;
  imageBarrierTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

  VkImageViewCreateInfo imageViewInfo{};
  imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewInfo.pNext = nullptr;
  imageViewInfo.flags = 0;
  imageViewInfo.image = image;
  imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
  imageViewInfo.subresourceRange.baseMipLevel = 0;
  imageViewInfo.subresourceRange.baseArrayLayer = 0;
  imageViewInfo.subresourceRange.layerCount = 1;
  imageViewInfo.subresourceRange.levelCount = 1;
  imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  checkForVulkanError(
      vkCreateImageView(device, &imageViewInfo, nullptr, &imageView),
      "Failed to create image view");

  VkSamplerCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;

  createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

  createInfo.minFilter = VK_FILTER_NEAREST;
  createInfo.magFilter = VK_FILTER_NEAREST;
  checkForVulkanError(vkCreateSampler(device, &createInfo, nullptr, &sampler),
                      "Failed to create 2D texture sampler");

  LOG_DEBUG("[Vulkan] 2D Texture created");

  auto texture = std::make_shared<Texture>(
      std::make_shared<VulkanTextureBinder>(device, allocator, image,
                                            allocation, imageView, sampler),
      textureSize, textureData.width, textureData.height, 1, textureData.format,
      statsManager);

  uploadContext.submit([extent, image, buffer](VkCommandBuffer commandBuffer) {
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = 1;

    VkImageMemoryBarrier imageBarrierTransfer{};
    imageBarrierTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrierTransfer.pNext = nullptr;
    imageBarrierTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageBarrierTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrierTransfer.image = image;
    imageBarrierTransfer.subresourceRange = range;
    imageBarrierTransfer.srcAccessMask = 0;
    imageBarrierTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrierTransfer);

    VkBufferImageCopy copyRegion{};
    copyRegion.bufferImageHeight = 0;
    copyRegion.bufferOffset = 0;
    copyRegion.bufferRowLength = 0;
    copyRegion.imageExtent = extent;
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageSubresource.mipLevel = 0;

    vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &copyRegion);

    VkImageMemoryBarrier imageBarrierReadable = imageBarrierTransfer;
    imageBarrierReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrierReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrierReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrierReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &imageBarrierReadable);
  });

  return texture;
}

SharedPtr<Texture> VulkanResourceAllocator::createTextureCubemap(
    const TextureCubemapData &textureData) {
  static constexpr uint32_t CUBEMAP_SIDES = 6;

  VkFormat format = static_cast<VkFormat>(textureData.format);

  auto hardwareBuffer =
      createHardwareBuffer(HardwareBuffer::Transfer, textureData.size);
  VkBuffer buffer = hardwareBuffer->getBuffer();
  void *data = hardwareBuffer->map();
  for (size_t i = 0; i < CUBEMAP_SIDES; ++i) {
    memcpy(static_cast<char *>(data) + textureData.faceData.at(i).size * i,
           textureData.data + textureData.faceData.at(i).offset,
           textureData.faceData.at(i).size);
  }
  hardwareBuffer->unmap();

  VkExtent3D extent{};
  extent.width = textureData.width;
  extent.height = textureData.height;
  extent.depth = 1;

  VmaAllocation allocation = nullptr;
  VkImage image = VK_NULL_HANDLE;
  VkImageView imageView = VK_NULL_HANDLE;
  VkSampler sampler = VK_NULL_HANDLE;

  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.format = format;
  imageCreateInfo.extent = extent;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.arrayLayers = CUBEMAP_SIDES;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageCreateInfo.usage =
      VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

  VmaAllocationCreateInfo imageAllocationInfo{};
  imageAllocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  checkForVulkanError(vmaCreateImage(allocator, &imageCreateInfo,
                                     &imageAllocationInfo, &image, &allocation,
                                     nullptr),
                      "Failed to create cubemap image");

  uploadContext.submit([extent, buffer, image,
                        &textureData](VkCommandBuffer commandBuffer) {
    VkImageSubresourceRange range{};
    range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    range.baseMipLevel = 0;
    range.levelCount = 1;
    range.baseArrayLayer = 0;
    range.layerCount = CUBEMAP_SIDES;

    VkImageMemoryBarrier imageBarrierTransfer{};
    imageBarrierTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrierTransfer.pNext = nullptr;
    imageBarrierTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageBarrierTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrierTransfer.image = image;
    imageBarrierTransfer.subresourceRange = range;
    imageBarrierTransfer.srcAccessMask = 0;
    imageBarrierTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0,
                         nullptr, 1, &imageBarrierTransfer);

    std::array<VkBufferImageCopy, CUBEMAP_SIDES> copyRegions{};
    for (size_t i = 0; i < CUBEMAP_SIDES; ++i) {
      copyRegions.at(i) = {};
      copyRegions.at(i).bufferImageHeight = 0;
      copyRegions.at(i).bufferOffset = textureData.faceData.at(i).size * i;
      copyRegions.at(i).bufferRowLength = 0;
      copyRegions.at(i).imageExtent = extent;
      copyRegions.at(i).imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      copyRegions.at(i).imageSubresource.baseArrayLayer =
          static_cast<uint32_t>(i);
      copyRegions.at(i).imageSubresource.layerCount = 1;
      copyRegions.at(i).imageSubresource.mipLevel = 0;
    }

    vkCmdCopyBufferToImage(commandBuffer, buffer, image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, CUBEMAP_SIDES,
                           copyRegions.data());

    VkImageMemoryBarrier imageBarrierReadable = imageBarrierTransfer;
    imageBarrierReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageBarrierReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageBarrierReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageBarrierReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT,
                         VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &imageBarrierReadable);
  });

  VkImageViewCreateInfo imageViewInfo{};
  imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewInfo.pNext = nullptr;
  imageViewInfo.flags = 0;
  imageViewInfo.image = image;
  imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
  imageViewInfo.format = format;
  imageViewInfo.components = {
      VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
      VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
  imageViewInfo.subresourceRange.baseMipLevel = 0;
  imageViewInfo.subresourceRange.levelCount = 1;
  imageViewInfo.subresourceRange.baseArrayLayer = 0;
  imageViewInfo.subresourceRange.layerCount = CUBEMAP_SIDES;
  imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

  checkForVulkanError(
      vkCreateImageView(device, &imageViewInfo, nullptr, &imageView),
      "Failed to create image view");

  VkSamplerCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  createInfo.pNext = nullptr;
  createInfo.flags = 0;
  createInfo.minFilter = VK_FILTER_LINEAR;
  createInfo.magFilter = VK_FILTER_LINEAR;
  createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  createInfo.compareOp = VK_COMPARE_OP_NEVER;
  createInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;

  checkForVulkanError(vkCreateSampler(device, &createInfo, nullptr, &sampler),
                      "Failed to create cubemap texture sampler");

  auto binder = std::make_shared<VulkanTextureBinder>(
      device, allocator, image, allocation, imageView, sampler);

  LOG_DEBUG("[Vulkan] Cubemap texture created");
  return std::make_shared<Texture>(binder, textureData.size, textureData.width,
                                   textureData.height, CUBEMAP_SIDES,
                                   textureData.format, statsManager);
}

SharedPtr<Texture> VulkanResourceAllocator::createTextureFramebuffer(
    const TextureFramebufferData &data) {
  size_t textureSize = data.width * data.height;

  VmaAllocation allocation = nullptr;
  VkImage image = VK_NULL_HANDLE;
  VkImageView imageView = VK_NULL_HANDLE;
  VkSampler sampler = VK_NULL_HANDLE;

  VkImageCreateInfo imageCreateInfo{};
  imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageCreateInfo.pNext = nullptr;
  imageCreateInfo.flags = 0;
  imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
  imageCreateInfo.extent = {data.width, data.height, 1};
  imageCreateInfo.arrayLayers = data.layers;
  imageCreateInfo.mipLevels = 1;
  imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  imageCreateInfo.format = static_cast<VkFormat>(data.format);
  imageCreateInfo.usage = static_cast<VkImageUsageFlags>(data.usageFlags);

  VmaAllocationCreateInfo allocationCreateInfo{};
  allocationCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

  checkForVulkanError(vmaCreateImage(allocator, &imageCreateInfo,
                                     &allocationCreateInfo, &image, &allocation,
                                     nullptr),
                      "Failed to create shadow map image");

  VkImageViewCreateInfo imageViewCreateInfo{};
  imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewCreateInfo.flags = 0;
  imageViewCreateInfo.pNext = nullptr;
  imageViewCreateInfo.format = static_cast<VkFormat>(data.format);
  imageViewCreateInfo.viewType =
      data.layers > 1 ? VK_IMAGE_VIEW_TYPE_2D_ARRAY : VK_IMAGE_VIEW_TYPE_2D;
  imageViewCreateInfo.image = image;
  imageViewCreateInfo.subresourceRange = {};
  imageViewCreateInfo.subresourceRange.aspectMask =
      static_cast<VkImageAspectFlags>(data.aspectMask);
  imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
  imageViewCreateInfo.subresourceRange.levelCount = 1;
  imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
  imageViewCreateInfo.subresourceRange.layerCount = data.layers;

  checkForVulkanError(
      vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView),
      "Failed to create shadow map image view");

  VkSamplerCreateInfo samplerCreateInfo{};
  samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  samplerCreateInfo.flags = 0;
  samplerCreateInfo.pNext = nullptr;
  samplerCreateInfo.minFilter = VK_FILTER_NEAREST;
  samplerCreateInfo.magFilter = VK_FILTER_NEAREST;
  samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
  samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  samplerCreateInfo.mipLodBias = 0.0f;
  samplerCreateInfo.maxAnisotropy = 1.0f;
  samplerCreateInfo.minLod = 0.0f;
  samplerCreateInfo.maxLod = 1.0f;
  samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

  checkForVulkanError(
      vkCreateSampler(device, &samplerCreateInfo, nullptr, &sampler),
      "Failed to create shadow map sampler");

  auto binder = std::make_shared<VulkanTextureBinder>(
      device, allocator, image, allocation, imageView, sampler);

  return std::make_shared<Texture>(binder, textureSize, data.width, data.height,
                                   data.layers, data.format, statsManager);
}

} // namespace liquid