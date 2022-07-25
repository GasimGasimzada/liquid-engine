#pragma once

#include "liquid/renderer/Material.h"

namespace liquid {

/**
 * @brief Material asset data
 */
struct MaterialAsset {
  /**
   * Base color texture
   */
  TextureAssetHandle baseColorTexture = TextureAssetHandle::Invalid;

  /**
   * Base color texture coordinate index
   */
  int8_t baseColorTextureCoord = -1;

  /**
   * Base color factor
   */
  glm::vec4 baseColorFactor{};

  /**
   * Metallic roughness textures
   */
  TextureAssetHandle metallicRoughnessTexture = TextureAssetHandle::Invalid;

  /**
   * Metallic roughness texture coordinate index
   */
  int8_t metallicRoughnessTextureCoord = -1;

  /**
   * Metallic factor
   */
  float metallicFactor = 0.0f;

  /**
   * Roughness factor
   */
  float roughnessFactor = 0.0f;

  /**
   * Normal texture
   */
  TextureAssetHandle normalTexture = TextureAssetHandle::Invalid;

  /**
   * Normal texture coordinate index
   */
  int8_t normalTextureCoord = -1;

  /**
   * Normal scale
   */
  float normalScale = 0.0f;

  /**
   * Occlusion texture
   */
  TextureAssetHandle occlusionTexture = TextureAssetHandle::Invalid;

  /**
   * Occlusion texture coordinate index
   */
  int8_t occlusionTextureCoord = -1;

  /**
   * Occlusion strength
   */
  float occlusionStrength = 0.0f;

  /**
   * Emissive texture
   */
  TextureAssetHandle emissiveTexture = TextureAssetHandle::Invalid;

  /**
   * Emissive texture coordinate index
   */
  int8_t emissiveTextureCoord = -1;

  /**
   * Emissive factor
   */
  glm::vec3 emissiveFactor{};

  /**
   * Material device handle
   */
  SharedPtr<Material> deviceHandle;
};

} // namespace liquid
