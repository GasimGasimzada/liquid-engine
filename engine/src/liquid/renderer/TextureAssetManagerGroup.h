#pragma once

#include "liquid/asset/Result.h"
#include "liquid/asset/AssetManagerGroup.h"
#include "liquid/asset/AssetRegistry.h"

#include "liquid/renderer/TextureAsset.h"

namespace liquid {

class TextureAssetLoader {
public:
  using AssetHandle = TextureAssetHandle;

  Result<TextureAssetHandle> loadFromPath(const Path &path,
                                          AssetRegistry &registry,
                                          const Path &assetsPath);

  Result<TextureAssetHandle> getOrLoadFromPath(StringView relativePath,
                                               AssetRegistry &registry,
                                               const Path &assetsPath);
};

class TextureAssetCreator {
public:
  using Asset = TextureAsset;

  Result<Path> createAssetFile(const AssetData<TextureAsset> &asset,
                               AssetRegistry &registry, const Path &assetsPath);
};

using TextureAssetManagerGroup =
    AssetManagerGroup<TextureAssetLoader, TextureAssetCreator>;

} // namespace liquid
