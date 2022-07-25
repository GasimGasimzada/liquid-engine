#pragma once

#include "liquid/rhi/RenderCommandList.h"
#include "liquid/rhi/RenderGraph.h"
#include "liquid/asset/AssetRegistry.h"
#include "RenderStorage.h"
#include "ShaderLibrary.h"

namespace liquid {

/**
 * @brief Scene render pass data
 */
struct SceneRenderPassData {
  /**
   * @brief Scene texture
   */
  rhi::TextureHandle sceneColor;

  /**
   * @brief Scene depth buffer
   */
  rhi::TextureHandle depthBuffer;
};

/**
 * @brief Scene renderer
 */
class SceneRenderer {
public:
  /**
   * @brief Create scene renderer
   *
   * @param shaderLibrary Shader library
   * @param resourceRegistry Resource registry
   * @param assetRegistry Asset registry
   */
  SceneRenderer(ShaderLibrary &shaderLibrary,
                rhi::ResourceRegistry &resourceRegistry,
                AssetRegistry &assetRegistry);

  /**
   * @brief Attach passes to render graph
   *
   * @param graph Render graph
   * @return Scene render pass data
   */
  SceneRenderPassData attach(rhi::RenderGraph &graph);

  /**
   * @brief Update frame data
   *
   * @param entityDatabase Entity database
   * @param camera Camera entity
   */
  void updateFrameData(EntityDatabase &entityDatabase, Entity camera);

private:
  /**
   * @brief Render meshes
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param bindMaterialData Bind material data
   */
  void render(rhi::RenderCommandList &commandList, rhi::PipelineHandle pipeline,
              bool bindMaterialData = false);

  /**
   * @brief Render skinned meshes
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   * @param bindMaterialData Bind material data
   */
  void renderSkinned(rhi::RenderCommandList &commandList,
                     rhi::PipelineHandle pipeline,
                     bool bindMaterialData = true);

  /**
   * @brief Render texts
   *
   * @param commandList Command list
   * @param pipeline Pipeline handle
   */
  void renderText(rhi::RenderCommandList &commandList,
                  rhi::PipelineHandle pipeline);

private:
  ShaderLibrary &mShaderLibrary;
  rhi::ResourceRegistry &mRegistry;
  RenderStorage mRenderStorage;
  AssetRegistry &mAssetRegistry;
};

} // namespace liquid
