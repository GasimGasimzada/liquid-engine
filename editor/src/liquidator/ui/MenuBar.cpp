#include "liquid/core/Base.h"
#include "MenuBar.h"
#include "ConfirmationDialog.h"

#include "liquid/imgui/Imgui.h"
#include "Widgets.h"

namespace liquidator {

void MenuBar::render(EditorManager &editorManager,
                     EntityManager &entityManager) {
  if (ImGui::BeginMenu("Objects")) {
    if (ImGui::MenuItem("Create empty entity", nullptr)) {
      entityManager.createEmptyEntity(editorManager.getEditorCamera(),
                                      liquid::EntityNull);
    }
    ImGui::EndMenu();
  }
}

} // namespace liquidator
