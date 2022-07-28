#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetManager.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

#include <ktx.h>
#include <vulkan/vulkan.h>
#include <ktxvulkan.h>
#include "liquid/loaders/KtxError.h"

namespace liquid {

Result<Path>
AssetManager::createTextureFromAsset(const AssetData<TextureAsset> &asset) {
  return textures().createAssetFile(asset);
}

Result<TextureAssetHandle>
AssetManager::loadTextureFromFile(const Path &filePath) {
  return textures().loadFromPath(filePath);
}

Result<TextureAssetHandle>
AssetManager::getOrLoadTextureFromPath(StringView relativePath) {
  if (relativePath.empty()) {
    return Result<TextureAssetHandle>::Ok(TextureAssetHandle::Invalid);
  }

  Path fullPath = (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getTextures().getAssets()) {
    if (asset.path == fullPath) {
      return Result<TextureAssetHandle>::Ok(handle);
    }
  }

  return loadTextureFromFile(fullPath);
}

} // namespace liquid
