#pragma once

#include "liquid/asset/Result.h"
#include "liquid/asset/AssetManagerGroup.h"
#include "liquid/asset/AssetRegistry.h"
#include "liquid/asset/InputBinaryStream.h"

#include "liquid/renderer/MaterialAsset.h"

namespace liquid {

class MaterialAssetLoader {
public:
  using AssetHandle = MaterialAssetHandle;
  using Stream = InputBinaryStream;

  Result<MaterialAssetHandle> loadFromPath(const Path &path,
                                           AssetRegistry &registry,
                                           const Path &assetsPath);

  Result<MaterialAssetHandle> loadFromStream(InputBinaryStream &stream,
                                             const Path &filePath,
                                             AssetRegistry &registry,
                                             const Path &assetsPath);

  Result<MaterialAssetHandle> getOrLoadFromPath(StringView relativePath,
                                                AssetRegistry &registry,
                                                const Path &assetsPath);
};

class MaterialAssetCreator {
public:
  using Asset = MaterialAsset;

  Result<Path> createAssetFile(const AssetData<MaterialAsset> &asset,
                               AssetRegistry &registry, const Path &assetsPath);
};

using MaterialAssetManagerGroup =
    AssetManagerGroup<MaterialAssetLoader, MaterialAssetCreator>;

} // namespace liquid
