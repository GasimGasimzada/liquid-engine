#pragma once

#include "../editor-scene/SceneManager.h"

#include "MenuBar.h"
#include "SceneHierarchyPanel.h"
#include "EntityPanel.h"
#include "EditorGridPanel.h"
#include "StatusBar.h"
#include "Layout.h"
#include "liquid/animation/AnimationSystem.h"
#include "liquid/physics/PhysicsSystem.h"

namespace liquidator {

class UIRoot {
public:
  /**
   * @brief Create UI Root
   *
   * @param context Entity context
   * @param gltfLoader GLTF loader
   */
  UIRoot(liquid::EntityContext &context, const liquid::GLTFLoader &gltfLoader);

  /**
   * @brief Render UI Root
   *
   * Renders all components inside the root
   *
   * @param sceneManager Scene manager
   * @param animationSystem Animation system
   * @param physicsSystem Physics system
   */
  void render(SceneManager &sceneManager,
              const liquid::AnimationSystem &animationSystem,
              liquid::PhysicsSystem &physicsSystem);

private:
  /**
   * @brief Handle node click from scene hierarchy panel
   *
   * @param node Clicked node item
   */
  void handleNodeClick(liquid::SceneNode *node);

private:
  MenuBar menuBar;
  SceneHierarchyPanel sceneHierarchyPanel;
  EntityPanel entityPanel;
  EditorGridPanel editorCameraPanel;
  StatusBar statusBar;
  Layout layout;
};

} // namespace liquidator
