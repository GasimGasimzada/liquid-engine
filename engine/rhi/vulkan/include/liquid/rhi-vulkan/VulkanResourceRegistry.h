#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid::rhi {

class VulkanBuffer;
class VulkanTexture;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanPipeline;
class VulkanShader;

/**
 * @brief Vulkan resource registry
 *
 * Stores all the resources associated
 * with a device
 */
class VulkanResourceRegistry {
  template <class THandle, class TVulkanObject>
  using VulkanResourceMap =
      std::unordered_map<THandle, std::unique_ptr<TVulkanObject>>;

  using ShaderMap = VulkanResourceMap<ShaderHandle, VulkanShader>;
  using BufferMap = VulkanResourceMap<BufferHandle, VulkanBuffer>;
  using TextureMap = VulkanResourceMap<TextureHandle, VulkanTexture>;
  using RenderPassMap =
      VulkanResourceMap<rhi::RenderPassHandle, VulkanRenderPass>;
  using FramebufferMap =
      VulkanResourceMap<FramebufferHandle, VulkanFramebuffer>;
  using PipelineMap = VulkanResourceMap<PipelineHandle, VulkanPipeline>;

public:
  /**
   * @brief Set shader
   *
   * @param handle Shader handle
   * @param shader Vulkan shader
   */
  void setShader(ShaderHandle handle, std::unique_ptr<VulkanShader> &&shader);

  /**
   * @brief Delete shader
   *
   * @param handle Shader handle
   */
  void deleteShader(ShaderHandle handle);

  /**
   * @brief Get shaders
   *
   * @return List of shaders
   */
  inline const ShaderMap &getShaders() const { return mShaders; }

  /**
   * @brief Set buffer
   *
   * @param handle Buffer handle
   * @param buffer Vulkan buffer
   */
  void setBuffer(BufferHandle handle, std::unique_ptr<VulkanBuffer> &&buffer);

  /**
   * @brief Delete buffer
   *
   * @param handle Buffer handle
   */
  void deleteBuffer(BufferHandle handle);

  /**
   * @brief Check if buffer exists
   *
   * @param handle Buffer handle
   * @retval true Buffer exists
   * @retval false Buffer does not exist
   */
  inline bool hasBuffer(BufferHandle handle) const {
    return mBuffers.find(handle) != mBuffers.end();
  }

  /**
   * @brief Get buffers
   *
   * @return List of buffers
   */
  inline const BufferMap &getBuffers() const { return mBuffers; }

  /**
   * @brief Set texture
   *
   * @param handle Texture handle
   * @param texture Vulkan texture
   */
  void setTexture(TextureHandle handle,
                  std::unique_ptr<VulkanTexture> &&texture);

  /**
   * @brief Delete texture
   *
   * @param handle Texture handle
   */
  void deleteTexture(TextureHandle handle);

  /**
   * @brief Delete dangling swapchain relative textures
   *
   * If a texture does not exist or is not swapchain
   * relative, remove it from the swapchain relative
   * textures list
   */
  void deleteDanglingSwapchainRelativeTextures();

  /**
   * @brief Get swapchain relative textures
   *
   * @return Swapchain relative textures
   */
  inline const std::set<TextureHandle> &getSwapchainRelativeTextures() const {
    return mSwapchainRelativeTextures;
  }

  /**
   * @brief Get textures
   *
   * @return List of textures
   */
  inline const TextureMap &getTextures() const { return mTextures; }

  /**
   * @brief Set render pass
   *
   * @param handle Render pass handle
   * @param renderPass Vulkan render pass
   */
  void setRenderPass(rhi::RenderPassHandle handle,
                     std::unique_ptr<VulkanRenderPass> &&renderPass);

  /**
   * @brief Delete render pass
   *
   * @param handle Render pass handle
   */
  void deleteRenderPass(rhi::RenderPassHandle handle);

  /**
   * @brief Get render passes
   *
   * @return List of render passes
   */
  inline const RenderPassMap &getRenderPasses() const { return mRenderPasses; }

  /**
   * @brief Set framebuffer
   *
   * @param handle Framebuffer handle
   * @param framebuffer Vulkan framebuffer
   */
  void setFramebuffer(FramebufferHandle handle,
                      std::unique_ptr<VulkanFramebuffer> &&framebuffer);

  /**
   * @brief Delete framebuffer
   *
   * @param handle Framebuffer handle
   */
  void deleteFramebuffer(rhi::FramebufferHandle handle);

  /**
   * @brief Get framebuffers
   *
   * @return List of framebuffers
   */
  inline const FramebufferMap &getFramebuffers() const { return mFramebuffers; }

  /**
   * @brief Set pipeline
   *
   * @param handle Pipeline handle
   * @param pipeline Vulkan pipeline
   */
  void setPipeline(PipelineHandle handle,
                   std::unique_ptr<VulkanPipeline> &&pipeline);

  /**
   * @brief Delete pipeline
   *
   * @param handle Pipeline handle
   */
  void deletePipeline(rhi::PipelineHandle handle);

  /**
   * @brief Get pipelines
   *
   * @return List of pipelines
   */
  inline const PipelineMap &getPipelines() const { return mPipelines; }

private:
  ShaderMap mShaders;
  BufferMap mBuffers;
  TextureMap mTextures;
  RenderPassMap mRenderPasses;
  FramebufferMap mFramebuffers;
  PipelineMap mPipelines;

  std::set<TextureHandle> mSwapchainRelativeTextures;
};

} // namespace liquid::rhi
