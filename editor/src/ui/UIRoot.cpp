#include "liquid/core/Base.h"
#include "UIRoot.h"

namespace liquidator {

UIRoot::UIRoot(liquid::EntityContext &context,
               const liquid::GLTFLoader &gltfLoader)
    : menuBar(gltfLoader), sceneHierarchyPanel(context), entityPanel(context) {
  sceneHierarchyPanel.setNodeClickHandler(
      [this](liquid::SceneNode *node) { handleNodeClick(node); });
}

void UIRoot::render(SceneManager &sceneManager,
                    const liquid::AnimationSystem &animationSystem) {
  layout.setup();
  menuBar.render(sceneManager);
  statusBar.render(sceneManager);
  sceneHierarchyPanel.render(sceneManager);
  entityPanel.render(sceneManager, animationSystem);
  editorCameraPanel.render(sceneManager);
}

void UIRoot::handleNodeClick(liquid::SceneNode *node) {
  entityPanel.setSelectedEntity(node->getEntity());
}

} // namespace liquidator
