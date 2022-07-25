#pragma once

namespace liquid {

/**
 * @brief Lua script asset data
 */
struct LuaScriptAsset {
  /**
   * Bytes that represent the script
   */
  std::vector<char> bytes;
};

} // namespace liquid
