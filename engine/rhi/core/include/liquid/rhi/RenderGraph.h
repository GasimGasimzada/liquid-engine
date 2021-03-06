#pragma once

#include "RenderGraphPass.h"
#include "ResourceRegistry.h"

namespace liquid::rhi {

/**
 * @brief Render graph
 */
class RenderGraph {
public:
  /**
   * @brief Add pass
   *
   * @param name Pass name
   * @return Render graph pass
   */
  RenderGraphPass &addPass(StringView name);

  /**
   * @brief Compile render graph
   *
   * Topologically sorts and updates render
   * passes in place
   *
   * @param resourceRegistry Resource registry
   */
  void compile(ResourceRegistry &resourceRegistry);

  /**
   * @brief Get passes
   *
   * @return Render graph passes
   */
  inline std::vector<RenderGraphPass> &getPasses() { return mPasses; }

  /**
   * @brief Get compiled passes
   *
   * @return Compiled render graph passes
   */
  inline std::vector<RenderGraphPass> &getCompiledPasses() {
    return mCompiledPasses;
  }

  /**
   * @brief Set framebuffer extent
   *
   * @param framebufferExtent Framebuffer extent
   */
  void setFramebufferExtent(glm::uvec2 framebufferExtent);

  /**
   * @brief Get framebuffer extent
   *
   * @return Framebuffer extent
   */
  inline const glm::uvec2 &getFramebufferExtent() const {
    return mFramebufferExtent;
  }

  /**
   * @brief Check if recreate is necessary
   *
   * @retval true Passes have changed
   * @retval false Passes have not changed
   */
  inline bool isDirty() const { return mDirty; }

  /**
   * @brief Update dirty flag
   */
  void updateDirtyFlag();

private:
  std::vector<RenderGraphPass> mPasses;
  std::vector<RenderGraphPass> mCompiledPasses;

  glm::uvec2 mFramebufferExtent{};
  bool mDirty = true;
};

} // namespace liquid::rhi
