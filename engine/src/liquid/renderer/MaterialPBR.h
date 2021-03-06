#pragma once

#include "Material.h"
#include "ShaderLibrary.h"

namespace liquid {

/**
 * @brief PBR material instance
 */
class MaterialPBR : public Material {
public:
  /**
   * @brief PBR material properties
   */
  struct Properties {
  public:
    /**
     * @brief Get textures based on properties
     *
     * @return List of textures
     */
    const std::vector<rhi::TextureHandle> getTextures() const;

    /**
     * @brief Get Property objects
     *
     * @return List of properties
     */
    const std::vector<std::pair<String, Property>> getProperties() const;

  public:
    /**
     * Base color texture
     */
    rhi::TextureHandle baseColorTexture = rhi::TextureHandle::Invalid;

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
    rhi::TextureHandle metallicRoughnessTexture = rhi::TextureHandle::Invalid;

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
    rhi::TextureHandle normalTexture = rhi::TextureHandle::Invalid;

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
    rhi::TextureHandle occlusionTexture = rhi::TextureHandle::Invalid;

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
    rhi::TextureHandle emissiveTexture = rhi::TextureHandle::Invalid;

    /**
     * Emissive texture coordinate index
     */
    int8_t emissiveTextureCoord = -1;

    /**
     * Emissive factor
     */
    glm::vec3 emissiveFactor{};
  };

public:
  /**
   * @brief Create PBR material
   *
   * @param properties PBR properties
   * @param registry Resource registry
   */
  MaterialPBR(const Properties &properties, rhi::ResourceRegistry &registry);
};

} // namespace liquid
