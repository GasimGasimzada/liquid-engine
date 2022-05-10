#pragma once

#include "../editor-scene/SceneManager.h"

#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"
#include "liquid/renderer/Renderer.h"

#include "MenuBar.h"
#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "StatusBar.h"
#include "AssetBrowser.h"
#include "Layout.h"
#include "IconRegistry.h"
#include "Inspector.h"
#include "EnvironmentPanel.h"

namespace liquidator {

/**
 * @brief Root UI entry point
 *
 * Renders all the UI in the scene
 */
class UIRoot {
public:
  /**
   * @brief Create UI Root
   *
   * @param entityContext Entity context
   * @param entityManager Entity manager
   * @param gltfImporter GLTF importer
   */
  UIRoot(liquid::EntityContext &entityContext, EntityManager &entityManager,
         GLTFImporter &gltfImporter);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param sceneManager Scene manager
   * @param renderer Renderer
   * @param assetManager Asset manager
   * @param physicsSystem Physics system
   */
  void render(SceneManager &sceneManager, liquid::Renderer &renderer,
              liquid::AssetManager &assetManager,
              liquid::PhysicsSystem &physicsSystem);

  /**
   * @brief Get icon registry
   *
   * @return Icon registry
   */
  inline IconRegistry &getIconRegistry() { return mIconRegistry; }

  /**
   * @brief Get asset browser panel
   *
   * @return Asset browser panel
   */
  inline AssetBrowser &getAssetBrowser() { return mAssetBrowser; }

private:
  Inspector mInspector;
  MenuBar mMenuBar;
  SceneHierarchyPanel mSceneHierarchyPanel;
  EntityPanel mEntityPanel;
  EnvironmentPanel mEnvironmentPanel;
  EditorGridPanel mEditorCameraPanel;
  AssetBrowser mAssetBrowser;
  StatusBar mStatusBar;
  Layout mLayout;
  IconRegistry mIconRegistry;
};

} // namespace liquidator
