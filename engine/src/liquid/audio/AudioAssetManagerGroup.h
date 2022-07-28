#pragma once

#include "liquid/asset/Result.h"
#include "liquid/asset/AssetManagerGroup.h"
#include "liquid/asset/AssetRegistry.h"

namespace liquid {

class AudioAssetLoader {
public:
  using AssetHandle = AudioAssetHandle;

  Result<AudioAssetHandle> loadFromPath(const Path &path,
                                        AssetRegistry &registry,
                                        const Path &assetsPath);
};

using AudioAssetManagerGroup =
    AssetManagerGroup<AudioAssetLoader, std::nullptr_t>;

} // namespace liquid
