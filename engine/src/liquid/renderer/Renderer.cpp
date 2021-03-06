#include "liquid/core/Base.h"
#include "liquid/core/Engine.h"
#include "liquid/core/EngineGlobals.h"
#include "liquid/window/Window.h"

#include "Renderer.h"
#include "StandardPushConstants.h"

#include "liquid/renderer/SceneRenderer.h"

#include "liquid/rhi/RenderGraph.h"

namespace liquid {

Renderer::Renderer(AssetRegistry &assetRegistry, Window &window,
                   rhi::RenderDevice *device)
    : mGraphEvaluator(mRegistry), mDevice(device),
      mImguiRenderer(window, mShaderLibrary, mRegistry),
      mAssetRegistry(assetRegistry),
      mSceneRenderer(mShaderLibrary, mRegistry, mAssetRegistry) {}

void Renderer::render(rhi::RenderGraph &graph,
                      rhi::RenderCommandList &commandList) {

  graph.compile(mRegistry);

  mGraphEvaluator.build(graph);

  mDevice->synchronize(mRegistry);
  mGraphEvaluator.execute(commandList, graph);
}

} // namespace liquid
