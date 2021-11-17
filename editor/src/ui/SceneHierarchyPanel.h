#pragma once

#include "scene/Scene.h"
#include "entity/EntityContext.h"
#include "../editor-scene/SceneManager.h"
#include "../editor-scene/EditorCamera.h"

namespace liquidator {

class SceneHierarchyPanel {
public:
  /**
   * @brief Create scene hierarchy panel
   *
   * @param context Entity context
   */
  SceneHierarchyPanel(liquid::EntityContext &context);

  /**
   * @brief Render the UI
   *
   * @param sceneManager Scene manager
   */
  void render(SceneManager &sceneManager);

private:
  /**
   * @brief Render scene node as tree node
   *
   * @param node Scene node
   * @param flags Flags
   * @param sceneManager Scene manager
   */
  void renderNode(liquid::SceneNode *node, int flags,
                  SceneManager &sceneManager);

  /**
   * @brief Handle node deletion
   *
   * @param node Scene node
   */
  void handleDelete(liquid::SceneNode *node);

  /**
   * @brief Handle moving to node
   *
   * @param node Scene node
   * @param camera Editor camera
   */
  void handleMoveToNode(liquid::SceneNode *node, EditorCamera &camera);

private:
  liquid::EntityContext &context;
};

} // namespace liquidator
