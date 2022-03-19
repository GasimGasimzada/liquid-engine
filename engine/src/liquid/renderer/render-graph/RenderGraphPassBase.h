#pragma once

#include "liquid/rhi/RenderCommandList.h"

#include "RenderGraphResource.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphRegistry.h"

#include "RenderGraphAttachmentDescription.h"

namespace liquid {

class RenderGraphPassBase {
public:
  /**
   * @brief Create render graph pass
   *
   * @param name Render pass name
   * @param renderPass Render pass resource
   */
  RenderGraphPassBase(const String &name, GraphResourceId renderPass);

  RenderGraphPassBase(const RenderGraphPassBase &) = delete;
  RenderGraphPassBase(RenderGraphPassBase &&) = delete;
  RenderGraphPassBase &operator=(const RenderGraphPassBase &) = delete;
  RenderGraphPassBase &operator=(RenderGraphPassBase &&) = delete;

  /**
   * @brief Default virtual destructor
   */
  virtual ~RenderGraphPassBase() = default;

  /**
   * @brief Build pass
   *
   * Checks for dirty flag and calls buildInternal
   *
   * @param builder Graph builder
   */
  void build(RenderGraphBuilder &&builder);

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   * @pure
   */
  virtual void buildInternal(RenderGraphBuilder &builder) = 0;

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   * @param registry Render graph registry
   * @pure
   */
  virtual void execute(rhi::RenderCommandList &commandList,
                       RenderGraphRegistry &registry) = 0;

  /**
   * @brief Get pass name
   *
   * @return Pass name
   */
  inline const String &getName() const { return mName; }

  /**
   * @brief Add input resource
   *
   * @param resourceId Input resource ID
   */
  void addInput(rhi::TextureHandle resourceId);

  /**
   * @brief Add output resource
   *
   * @param resourceId Output resource ID
   * @param attachment Attachment
   */
  void addOutput(rhi::TextureHandle resourceId,
                 const RenderPassAttachment &attachment);

  /**
   * @brief Add resource
   *
   * @param resourceId Non attachment resource ID
   */
  void addResource(GraphResourceId resourceId);

  /**
   * @brief Get all input resources
   *
   * @return Input resources
   */
  inline const std::vector<rhi::TextureHandle> &getInputs() const {
    return mInputs;
  }

  /**
   * @brief Get all output resources
   *
   * @return Output resources
   */
  inline std::unordered_map<rhi::TextureHandle, RenderPassAttachment> &
  getOutputs() {
    return mOutputs;
  }

  /**
   * @brief Get non attachment resources
   *
   * @return Resources
   */
  inline const std::vector<GraphResourceId> &getResources() const {
    return mResources;
  }

  /**
   * @brief Get render pass
   *
   * @return Render pass resource
   */
  inline GraphResourceId getRenderPass() const { return mRenderPass; }

  /**
   * @brief Check if pass is dirty and has to be rebuild
   *
   * @retval true Pass is dirty
   * @retval false Pass is not dirty
   */
  inline bool isDirty() const { return mDirty; }

private:
  std::vector<rhi::TextureHandle> mInputs;
  std::vector<GraphResourceId> mResources;
  std::unordered_map<rhi::TextureHandle, RenderPassAttachment> mOutputs;

  GraphResourceId mRenderPass;
  String mName;
  bool mDirty = true;
};

} // namespace liquid
