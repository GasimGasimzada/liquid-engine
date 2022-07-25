#pragma once

#include "liquid/rhi/RenderHandle.h"

namespace liquid {

enum class TextureAssetType { Standard, Cubemap };

static constexpr uint32_t DEFAULT_TEXTURE_FORMAT = 43;

/**
 * @brief Texture asset data
 */
struct TextureAsset {
  /**
   * Width
   */
  uint32_t width = 0;

  /**
   * Height
   */
  uint32_t height = 0;

  /**
   * Layers
   */
  uint32_t layers = 0;

  /**
   * Texture type
   */
  TextureAssetType type = TextureAssetType::Standard;

  /**
   * Texture format
   */
  uint32_t format = DEFAULT_TEXTURE_FORMAT;

  /**
   * Raw texture data
   */
  void *data = nullptr;

  /**
   * Device handle
   */
  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Invalid;
};

} // namespace liquid
