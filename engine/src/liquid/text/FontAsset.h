#pragma once

namespace liquid {

/**
 * @brief Font glyph data
 */
struct FontGlyph {

  /**
   * Atlas bounds
   */
  glm::vec4 bounds;

  /**
   * Quad bounds
   */
  glm::vec4 planeBounds;

  /**
   * Glyph advance
   */
  float advanceX = 0.0;
};

/**
 * @brief Font asset data
 */
struct FontAsset {
  /**
   * Font atlas raw data
   */
  std::vector<std::byte> atlas;

  /**
   * Font atlas dimensions
   */
  glm::uvec2 atlasDimensions;

  /**
   * Glyph data
   */
  std::unordered_map<uint32_t, FontGlyph> glyphs;

  /**
   * Font scale
   */
  float fontScale = 1.0f;

  /**
   * Device handle
   */
  rhi::TextureHandle deviceHandle = rhi::TextureHandle::Invalid;
};

} // namespace liquid
