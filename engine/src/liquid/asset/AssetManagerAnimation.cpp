#include "liquid/core/Base.h"
#include "liquid/core/Version.h"

#include "AssetManager.h"

#include "AssetFileHeader.h"
#include "OutputBinaryStream.h"
#include "InputBinaryStream.h"

namespace liquid {

Result<Path>
AssetManager::createAnimationFromAsset(const AssetData<AnimationAsset> &asset) {
  String extension = ".lqanim";
  Path assetPath = (mAssetsPath / (asset.name + extension)).make_preferred();
  OutputBinaryStream file(assetPath);

  if (!file.good()) {
    return Result<Path>::Error("File cannot be opened for writing: " +
                               assetPath.string());
  }

  AssetFileHeader header{};
  header.type = AssetType::Animation;
  header.version = createVersion(0, 1);
  file.write(header.magic, ASSET_FILE_MAGIC_LENGTH);
  file.write(header.version);
  file.write(header.type);

  file.write(asset.data.time);
  uint32_t numKeyframes = static_cast<uint32_t>(asset.data.keyframes.size());
  file.write(numKeyframes);

  for (auto &keyframe : asset.data.keyframes) {
    file.write(keyframe.target);
    file.write(keyframe.interpolation);
    file.write(keyframe.jointTarget);
    file.write(keyframe.joint);

    uint32_t numValues = static_cast<uint32_t>(keyframe.keyframeTimes.size());
    file.write(numValues);
    file.write(keyframe.keyframeTimes);
    file.write(keyframe.keyframeValues);
  }

  return Result<Path>::Ok(assetPath, {});
}

Result<AnimationAssetHandle>
AssetManager::loadAnimationDataFromInputStream(InputBinaryStream &stream,
                                               const Path &filePath) {
  AssetData<AnimationAsset> animation{};
  animation.path = filePath;
  animation.relativePath = std::filesystem::relative(filePath, mAssetsPath);
  animation.name = animation.relativePath.string();
  animation.type = AssetType::Animation;

  stream.read(animation.data.time);
  uint32_t numKeyframes = 0;
  stream.read(numKeyframes);
  animation.data.keyframes.resize(numKeyframes);

  for (auto &keyframe : animation.data.keyframes) {
    stream.read(keyframe.target);
    stream.read(keyframe.interpolation);
    stream.read(keyframe.jointTarget);
    stream.read(keyframe.joint);

    uint32_t numValues = 0;
    stream.read(numValues);
    keyframe.keyframeTimes.resize(numValues);
    keyframe.keyframeValues.resize(numValues);
    stream.read(keyframe.keyframeTimes);
    stream.read(keyframe.keyframeValues);
  }

  return Result<AnimationAssetHandle>::Ok(
      mRegistry.getAnimations().addAsset(animation));
}

Result<AnimationAssetHandle>
AssetManager::loadAnimationFromFile(const Path &filePath) {
  InputBinaryStream stream(filePath);

  const auto &header = checkAssetFile(stream, filePath, AssetType::Animation);
  if (header.hasError()) {
    return Result<AnimationAssetHandle>::Error(header.getError());
  }

  return loadAnimationDataFromInputStream(stream, filePath);
}

Result<AnimationAssetHandle>
AssetManager::getOrLoadAnimationFromPath(StringView relativePath) {
  if (relativePath.empty()) {
    return Result<AnimationAssetHandle>::Ok(AnimationAssetHandle::Invalid);
  }

  Path fullPath = (mAssetsPath / relativePath).make_preferred();

  for (auto &[handle, asset] : mRegistry.getAnimations().getAssets()) {
    if (asset.path == fullPath) {
      return Result<AnimationAssetHandle>::Ok(handle);
    }
  }

  return loadAnimationFromFile(fullPath);
}

} // namespace liquid
