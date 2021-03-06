#pragma once

#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

/**
 * @brief Loads images in various formats as textures
 *
 * Uses STB to load images in various formats
 */
class ImageTextureLoader {
public:
  /**
   * @brief Create image texture loader
   *
   * @param registry Resource registry
   */
  ImageTextureLoader(rhi::ResourceRegistry &registry);

  /**
   * @brief Load image from filename
   *
   * @param filename Filename
   * @return 2D Texture
   */
  rhi::TextureHandle loadFromFile(const String &filename);

private:
  rhi::ResourceRegistry &mRegistry;
};

} // namespace liquid
