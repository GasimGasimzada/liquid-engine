#pragma once

#include "liquid/asset/AssetRegistry.h"

namespace liquid {

namespace detail {

struct EmptyCreator {
  using Asset = std::nullptr_t;

  inline Result<Path> createAssetFile(const AssetData<std::nullptr_t> &asset,
                                      const Path &path) {
    return Result<Path>::Error("Not implemented");
  }
};

} // namespace detail

template <typename TLoader, typename TCreator> class AssetManagerGroup {
public:
  AssetManagerGroup(AssetRegistry &registry, const Path &assetsPath)
      : mRegistry(registry), mAssetsPath(assetsPath) {}

  template <typename T = TLoader, typename TAssetHandle = TLoader::AssetHandle>
  Result<TAssetHandle> loadFromPath(const Path &path) {
    static_assert(std::is_same_v<TLoader, T> &&
                      std::is_same_v<TAssetHandle, TLoader::AssetHandle>,
                  "Provided type should not be changed");
    return mLoader.loadFromPath(path, mRegistry, mAssetsPath);
  }

  template <typename T = TLoader, typename TAssetHandle = TLoader::AssetHandle,
            typename TStream = TLoader::Stream>
  Result<TAssetHandle> loadFromStream(TStream &stream, const Path &filePath) {
    static_assert(std::is_same_v<TLoader, T> &&
                      std::is_same_v<TAssetHandle, TLoader::AssetHandle> &&
                      std::is_same_v<TStream, TLoader::Stream>,
                  "Provided type should not be changed");
    return mLoader.loadFromStream(stream, filePath, mRegistry, mAssetsPath);
  }

  template <typename T = TCreator, typename TAsset = TCreator::Asset>
  Result<Path> createAssetFile(const AssetData<TAsset> &asset) {
    static_assert(!std::is_same_v<TCreator, std::nullptr_t>,
                  "Asset manager group does not specify a creator");
    static_assert(std::is_same_v<TCreator, T> &&
                      std::is_same_v<TAsset, TCreator::Asset>,
                  "Provided type should not be changed");
    return mCreator.createAssetFile(asset, mRegistry, mAssetsPath);
  }

private:
  AssetRegistry &mRegistry;
  const Path &mAssetsPath;

  TLoader mLoader;
  TCreator mCreator;
};

} // namespace liquid
