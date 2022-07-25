#pragma once

#include "liquid/audio/AudioAsset.h"

namespace liquid {

/**
 * @brief MiniAudio backend
 */
class MiniAudioBackend {
  class BackendImpl;

public:
  /**
   * @brief Create miniaudio engine
   */
  MiniAudioBackend();

  /**
   * @brief Destroy miniaudio engine
   */
  ~MiniAudioBackend();

  MiniAudioBackend(const MiniAudioBackend &) = delete;
  MiniAudioBackend(MiniAudioBackend &&) = delete;
  MiniAudioBackend &operator=(const MiniAudioBackend &) = delete;
  MiniAudioBackend &operator=(MiniAudioBackend &&) = delete;

  /**
   * @brief Play sound
   *
   * @param asset Audio asset data
   * @return Backend specific sound instance
   */
  void *playSound(const AudioAsset &asset);

  /**
   * @brief Destroy sound
   *
   * @param instance Backend specific sound instance
   */
  void destroySound(void *instance);

  /**
   * @brief Check if sound is playing
   *
   * @param instance Backend specific sound instance
   * @retval true Sound is playing
   * @retval false Sound is not playing
   */
  bool isPlaying(void *instance);

private:
  BackendImpl *mImpl = nullptr;
};

} // namespace liquid
