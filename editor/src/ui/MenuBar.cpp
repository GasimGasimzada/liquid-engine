#include "liquid/core/Base.h"
#include "MenuBar.h"
#include "ConfirmationDialog.h"

#include <imgui.h>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_access.hpp>

namespace liquidator {

void MenuBar::render(SceneManager &sceneManager) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Objects")) {
      if (ImGui::MenuItem("Create empty entity", nullptr)) {
        sceneManager.getEntityManager().createEmptyEntity(
            sceneManager.getEditorCamera(), liquid::ENTITY_MAX);
      }

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

} // namespace liquidator
