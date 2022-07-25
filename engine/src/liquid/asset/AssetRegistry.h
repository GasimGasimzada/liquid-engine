#pragma once

#include "Asset.h"
#include "AssetMap.h"

#include "liquid/renderer/MaterialAsset.h"
#include "liquid/renderer/TextureAsset.h"
#include "liquid/text/FontAsset.h"
#include "liquid/scene/MeshAsset.h"
#include "liquid/animation/SkeletonAsset.h"
#include "liquid/animation/AnimationAsset.h"
#include "liquid/audio/AudioAsset.h"
#include "liquid/scripting/LuaScriptAsset.h"
#include "PrefabAsset.h"

#include "liquid/rhi/ResourceRegistry.h"

namespace liquid {

/**
 * @brief Asset registry
 *
 * Stores all the loaded assets
 * in the engine
 */
class AssetRegistry {
  using TextureMap = AssetMap<TextureAssetHandle, TextureAsset>;
  using FontMap = AssetMap<FontAssetHandle, FontAsset>;
  using MaterialMap = AssetMap<MaterialAssetHandle, MaterialAsset>;
  using MeshMap = AssetMap<MeshAssetHandle, MeshAsset>;
  using SkinnedMeshMap = AssetMap<SkinnedMeshAssetHandle, SkinnedMeshAsset>;
  using SkeletonMap = AssetMap<SkeletonAssetHandle, SkeletonAsset>;
  using AnimationMap = AssetMap<AnimationAssetHandle, AnimationAsset>;
  using AudioMap = AssetMap<AudioAssetHandle, AudioAsset>;
  using PrefabMap = AssetMap<PrefabAssetHandle, PrefabAsset>;
  using LuaScriptMap = AssetMap<LuaScriptAssetHandle, LuaScriptAsset>;

  struct DefaultObjects {
    MeshAssetHandle cube = MeshAssetHandle::Invalid;
    MaterialAssetHandle defaultMaterial = MaterialAssetHandle::Invalid;
  };

public:
  AssetRegistry() = default;

  /**
   * @brief Destroy registry
   */
  ~AssetRegistry();

  AssetRegistry(const AssetRegistry &) = delete;
  AssetRegistry &operator=(const AssetRegistry &) = delete;
  AssetRegistry(AssetRegistry &&) = delete;
  AssetRegistry &operator=(AssetRegistry &&) = delete;

  /**
   * @brief Create default objects
   *
   * Handles for default objects are
   * stored for easy access
   */
  void createDefaultObjects();

  /**
   * @brief Get default objects
   *
   * @return Default objects
   */
  inline const DefaultObjects &getDefaultObjects() const {
    return mDefaultObjects;
  }

  /**
   * @brief Synchronize assets with device registry
   *
   * @param registry Device registry
   */
  void syncWithDeviceRegistry(rhi::ResourceRegistry &registry);

  /**
   * @brief Get textures
   *
   * @return Texture asset map
   */
  inline TextureMap &getTextures() { return mTextures; }

  /**
   * @brief Get fonts
   *
   * @return Font asset map
   */
  inline FontMap &getFonts() { return mFonts; }

  /**
   * @brief Get materials
   *
   * @return Material asset map
   */
  inline MaterialMap &getMaterials() { return mMaterials; }

  /**
   * @brief Get meshes
   *
   * @return Mesh asset map
   */
  inline MeshMap &getMeshes() { return mMeshes; }

  /**
   * @brief Get skinned meshes
   *
   * @return Skinned mesh asset map
   */
  inline SkinnedMeshMap &getSkinnedMeshes() { return mSkinnedMeshes; }

  /**
   * @brief Get skeletons
   *
   * @return Skeleton asset map
   */
  inline SkeletonMap &getSkeletons() { return mSkeletons; }

  /**
   * @brief Get animations
   *
   * @return Animation asset map
   */
  inline AnimationMap &getAnimations() { return mAnimations; }

  /**
   * @brief Get audios
   *
   * @return Audio asset map
   */
  inline AudioMap &getAudios() { return mAudios; }

  /**
   * @brief Get prefabs
   *
   * @return Prefab asset map
   */
  inline PrefabMap &getPrefabs() { return mPrefabs; }

  /**
   * @brief Get Lua scripts
   *
   * @return Lua script asset map
   */
  inline LuaScriptMap &getLuaScripts() { return mLuaScripts; }

  /**
   * @brief Get asset located at path
   *
   * @param filePath Path to asset
   * @retval {AssetType::None, 0} Asset does not exist
   * @return Asset type and ID
   */
  std::pair<AssetType, uint32_t> getAssetByPath(const Path &filePath);

private:
  TextureMap mTextures;
  FontMap mFonts;
  MaterialMap mMaterials;
  MeshMap mMeshes;
  SkinnedMeshMap mSkinnedMeshes;
  SkeletonMap mSkeletons;
  AnimationMap mAnimations;
  AudioMap mAudios;
  PrefabMap mPrefabs;
  LuaScriptMap mLuaScripts;

  DefaultObjects mDefaultObjects;
};

} // namespace liquid
