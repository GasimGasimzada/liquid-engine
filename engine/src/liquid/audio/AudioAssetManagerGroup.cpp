#include "liquid/core/Base.h"
#include "liquid/audio/MiniAudio.h"
#include "AudioAssetManagerGroup.h"

namespace liquid {

static AudioAssetFormat getAudioFormatFromExtension(StringView extension) {
  if (extension == "wav") {
    return AudioAssetFormat::Wav;
  }

  if (extension == "mp3") {
    return AudioAssetFormat::Mp3;
  }

  return AudioAssetFormat::Unknown;
}

Result<AudioAssetHandle>
AudioAssetLoader::loadFromPath(const Path &filePath, AssetRegistry &registry,
                               const Path &assetsPath) {
  auto ext = filePath.extension().string();
  ext.erase(0, 1);
  auto format = getAudioFormatFromExtension(ext);

  if (format == AudioAssetFormat::Unknown) {
    return Result<AudioAssetHandle>::Error("Cannot load audio file: " +
                                           filePath.string());
  }

  auto *decoder = new ma_decoder;

  std::ifstream stream(filePath, std::ios::binary | std::ios::ate);

  if (stream.bad()) {
    return Result<AudioAssetHandle>::Error("Cannot load audio file: " +
                                           filePath.string());
  }

  std::ifstream::pos_type pos = stream.tellg();

  if (pos <= 0) {
    return Result<AudioAssetHandle>::Error(
        "Could not open file: File is empty");
  }

  std::vector<char> bytes(pos);
  stream.seekg(0, std::ios::beg);
  stream.read(&bytes[0], pos);

  AssetData<AudioAsset> asset;
  asset.path = filePath;
  asset.relativePath = std::filesystem::relative(filePath, assetsPath);
  asset.name = asset.relativePath.string();
  asset.type = AssetType::Audio;
  asset.data.bytes = bytes;
  asset.data.format = format;

  return Result<AudioAssetHandle>::Ok(registry.getAudios().addAsset(asset));
}

} // namespace liquid
