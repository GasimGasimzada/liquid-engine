#include "liquid/core/Base.h"
#include "AssetManager.h"

namespace liquid {

Result<AudioAssetHandle> AssetManager::loadAudioFromFile(const Path &filePath) {
  return audios().loadFromPath(filePath);
}

} // namespace liquid
