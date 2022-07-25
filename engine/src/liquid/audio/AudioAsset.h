#pragma once

namespace liquid {

enum class AudioAssetFormat { Unknown = 0, Wav, Mp3 };

/**
 * @brief Audio asset data
 */
struct AudioAsset {
  /**
   * @brief Audio data
   */
  std::vector<char> bytes{};

  /**
   * @brief Audio asset format
   */
  AudioAssetFormat format = AudioAssetFormat::Unknown;
};

} // namespace liquid
