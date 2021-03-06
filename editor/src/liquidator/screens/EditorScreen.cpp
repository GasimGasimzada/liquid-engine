#include "liquid/core/Base.h"
#include "EditorScreen.h"

#include "liquid/renderer/Renderer.h"
#include "liquid/entity/EntityDatabase.h"
#include "liquid/renderer/StandardPushConstants.h"
#include "liquid/profiler/ImguiDebugLayer.h"
#include "liquid/scene/SceneUpdater.h"
#include "liquid/scene/SkeletonUpdater.h"
#include "liquid/scripting/ScriptingSystem.h"
#include "liquid/renderer/Presenter.h"
#include "liquid/asset/FileTracker.h"
#include "liquid/core/EntityDeleter.h"
#include "liquid/audio/AudioSystem.h"

#include "liquid/physics/PhysicsSystem.h"
#include "liquid/loop/MainLoop.h"
#include "liquid/imgui/ImguiUtils.h"
#include "liquid/scene/CameraAspectRatioUpdater.h"

#include "liquidator/editor-scene/EditorCamera.h"
#include "liquidator/editor-scene/EditorManager.h"
#include "liquidator/editor-scene/EditorGrid.h"
#include "liquidator/ui/UIRoot.h"
#include "liquidator/ui/AssetLoadStatusDialog.h"
#include "liquidator/ui/Theme.h"
#include "liquidator/ui/Widgets.h"

#include "liquidator/core/EditorRenderer.h"
#include "liquidator/core/EditorSimulator.h"

namespace liquidator {

EditorScreen::EditorScreen(liquid::Window &window,
                           liquid::EventSystem &eventSystem,
                           liquid::rhi::RenderDevice *device)
    : mWindow(window), mEventSystem(eventSystem), mDevice(device) {}

void EditorScreen::start(const Project &project) {
  liquid::FPSCounter fpsCounter;

  auto layoutPath = (project.settingsPath / "layout.ini").string();
  auto statePath = project.settingsPath / "state.lqstate";

  liquid::AssetManager assetManager(project.assetsPath);
  liquid::Renderer renderer(assetManager.getRegistry(), mWindow, mDevice);

  liquid::Presenter presenter(renderer.getShaderLibrary(),
                              renderer.getRegistry());

  presenter.updateFramebuffers(mDevice->getSwapchain());

  auto res = assetManager.preloadAssets(renderer.getRegistry());
  liquidator::AssetLoadStatusDialog preloadStatusDialog("Loaded with warnings");
  preloadStatusDialog.setMessages(res.getWarnings());

  Theme::apply();

  renderer.getImguiRenderer().useConfigPath(layoutPath);
  renderer.getImguiRenderer().setClearColor(
      Theme::getColor(ThemeColor::BackgroundColor));
  renderer.getImguiRenderer().buildFonts();

  renderer.getSceneRenderer().setClearColor(
      Theme::getColor(ThemeColor::SceneBackgroundColor));

  if (res.hasWarnings()) {
    preloadStatusDialog.show();
  }

  liquid::FileTracker tracker(project.assetsPath);
  tracker.trackForChanges();

  liquidator::EntityManager entityManager(assetManager, renderer,
                                          project.scenePath);
  liquidator::EditorCamera editorCamera(entityManager.getActiveEntityDatabase(),
                                        mEventSystem, renderer, mWindow);
  liquidator::EditorGrid editorGrid;
  liquidator::EditorManager editorManager(editorCamera, editorGrid,
                                          entityManager);

  editorManager.loadOrCreateScene();
  editorManager.loadEditorState(statePath);

  liquid::MainLoop mainLoop(mWindow, fpsCounter);
  liquidator::AssetLoader assetLoader(assetManager, renderer.getRegistry());

  liquid::ImguiDebugLayer debugLayer(mDevice->getDeviceInformation(),
                                     mDevice->getDeviceStats(),
                                     renderer.getRegistry(), fpsCounter);

  liquidator::UIRoot ui(entityManager, assetLoader);
  ui.getIconRegistry().loadIcons(renderer.getRegistry(),
                                 std::filesystem::current_path() / "assets" /
                                     "icons");

  mDevice->synchronize(renderer.getRegistry());

  liquidator::EditorRenderer editorRenderer(renderer.getRegistry(),
                                            renderer.getShaderLibrary(),
                                            ui.getIconRegistry());

  liquid::rhi::RenderGraph graph;

  auto scenePassGroup = renderer.getSceneRenderer().attach(graph);
  auto imguiPassGroup = renderer.getImguiRenderer().attach(graph);
  imguiPassGroup.pass.read(scenePassGroup.sceneColor);

  {
    static constexpr glm::vec4 BLUEISH_CLEAR_VALUE{0.52f, 0.54f, 0.89f, 1.0f};
    auto &pass = editorRenderer.attach(graph);
    pass.write(scenePassGroup.sceneColor, BLUEISH_CLEAR_VALUE);
    pass.write(scenePassGroup.depthBuffer,
               liquid::rhi::DepthStencilClear{1.0f, 0});
  }

  renderer.getSceneRenderer().attachText(graph, scenePassGroup);

  graph.setFramebufferExtent(mWindow.getFramebufferSize());

  mWindow.addResizeHandler([&graph](auto width, auto height) {
    graph.setFramebufferExtent({width, height});
  });

  mWindow.addFocusHandler(
      [&tracker, &assetManager, &renderer, &ui](bool focused) {
        if (!focused)
          return;

        const auto &changes = tracker.trackForChanges();
        for (auto &change : changes) {
          assetManager.loadAsset(change.path);
        }

        ui.getAssetBrowser().reload();
      });

  ui.getAssetBrowser().setOnCreateEntry(
      [&assetManager](auto path) { assetManager.loadAsset(path); });

  liquidator::EditorSimulator simulator(
      mEventSystem, mWindow, assetManager.getRegistry(), editorCamera);

  mainLoop.setUpdateFn(
      [&editorCamera, &entityManager, &simulator, this](float dt) mutable {
        auto &entityDatabase = entityManager.getActiveEntityDatabase();

        mEventSystem.poll();
        simulator.update(dt, entityDatabase);
        return true;
      });

  mainLoop.setRenderFn([&renderer, &editorManager, &entityManager,
                        &assetManager, &graph, &scenePassGroup, &imguiPassGroup,
                        &ui, &debugLayer, &preloadStatusDialog, &presenter,
                        &editorRenderer, &simulator, this]() {
    // TODO: Why is -2.0f needed here
    static const float IconSize = ImGui::GetFrameHeight() - 2.0f;

    auto &imgui = renderer.getImguiRenderer();
    auto &sceneRenderer = renderer.getSceneRenderer();

    imgui.beginRendering();

    if (widgets::MainMenuBar::begin()) {
      liquidator::MenuBar::render(editorManager, entityManager);
      debugLayer.renderMenu();
      widgets::MainMenuBar::end();
    }

    debugLayer.render();

    if (Toolbar::begin()) {
      auto icon =
          entityManager.isUsingSimulationDatabase()
              ? ui.getIconRegistry().getIcon(liquidator::EditorIcon::Stop)
              : ui.getIconRegistry().getIcon(liquidator::EditorIcon::Play);

      if (liquid::imgui::imageButton(icon, ImVec2(IconSize, IconSize))) {
        if (entityManager.isUsingSimulationDatabase()) {
          simulator.cleanupSimulationDatabase(
              entityManager.getActiveEntityDatabase());
          simulator.useEditorUpdate();
          entityManager.useEditingDatabase();
        } else {
          simulator.useSimulationUpdate();
          entityManager.useSimulationDatabase();
        }
      }

      ImGui::SameLine();

      if (!editorManager.isUsingEditorCamera() &&
          ImGui::Button("Reset to editor camera")) {
        editorManager.switchToEditorCamera();
      }
    }
    Toolbar::end();

    ui.render(editorManager, renderer, assetManager,
              simulator.getPhysicsSystem(), entityManager);

    if (SceneView::begin(scenePassGroup.sceneColor)) {
      const auto &pos = ImGui::GetWindowPos();
      const auto &size = ImGui::GetItemRectSize();

      editorManager.getEditorCamera().setViewport(pos.x, pos.y + IconSize,
                                                  size.x, size.y);
    }

    SceneView::end();
    StatusBar::render(editorManager);

    preloadStatusDialog.render();

    imgui.endRendering();

    const auto &renderFrame = mDevice->beginFrame();

    if (renderFrame.frameIndex < std::numeric_limits<uint32_t>::max()) {
      imgui.updateFrameData(renderFrame.frameIndex);
      sceneRenderer.updateFrameData(entityManager.getActiveEntityDatabase(),
                                    editorManager.getCamera());
      editorRenderer.updateFrameData(entityManager.getActiveEntityDatabase(),
                                     editorManager.getCamera(),
                                     editorManager.getEditorGrid());

      renderer.render(graph, renderFrame.commandList);

      presenter.present(renderFrame.commandList, imguiPassGroup.imguiColor,
                        renderFrame.swapchainImageIndex);

      mDevice->endFrame(renderFrame);
    } else {
      presenter.updateFramebuffers(mDevice->getSwapchain());
    }
  });

  mainLoop.run();
  editorManager.saveEditorState(statePath);
}

} // namespace liquidator
