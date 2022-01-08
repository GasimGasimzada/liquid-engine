#pragma once

#include "renderer/render-graph/RenderGraphPassBase.h"
#include "renderer/ShaderLibrary.h"
#include "entity/EntityContext.h"
#include "renderer/vulkan/VulkanRenderData.h"

namespace liquid {

class FullscreenQuadPass : public RenderGraphPassBase {
public:
  /**
   * @brief Create fullscreen quad pass
   *
   * @param name Pass name
   * @param renderPassId Render pass resource ID
   * @param shaderLibrary Shader library
   * @param inputDep Input dependency
   */
  FullscreenQuadPass(const String &name, GraphResourceId renderPassId,
                     ShaderLibrary *shaderLibrary, const String &inputDep);

  /**
   * @brief Build pass
   *
   * @param builder Graph builder
   */
  void buildInternal(RenderGraphBuilder &builder) override;

  /**
   * @brief Execute pass
   *
   * @param commandList Render command list
   * @param registry Render graph registry
   */
  void execute(RenderCommandList &commandList,
               RenderGraphRegistry &registry) override;

private:
  GraphResourceId pipelineId = 0;
  GraphResourceId inputTexture = 0;
  ShaderLibrary *shaderLibrary;
  String inputDep;
};

} // namespace liquid
