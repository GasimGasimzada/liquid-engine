#include "liquid/core/Base.h"
#include "AssetBrowser.h"

#include <imgui.h>

namespace liquidator {

static void ImguiImage(liquid::rhi::TextureHandle handle, const ImVec2 &size) {
  ImGui::Image(reinterpret_cast<void *>(static_cast<uintptr_t>(handle)), size);
}

static EditorIcon getIconFromAssetType(liquid::AssetType type) {
  switch (type) {
  case liquid::AssetType::Texture:
    return EditorIcon::Texture;
  case liquid::AssetType::Material:
    return EditorIcon::Material;
  case liquid::AssetType::Mesh:
  case liquid::AssetType::SkinnedMesh:
    return EditorIcon::Mesh;
  case liquid::AssetType::Skeleton:
    return EditorIcon::Skeleton;
  case liquid::AssetType::Animation:
    return EditorIcon::Animation;
  case liquid::AssetType::Prefab:
    return EditorIcon::Prefab;
  default:
    return EditorIcon::Unknown;
  }
}

AssetBrowser::AssetBrowser(GLTFImporter &gltfImporter)
    : mGltfImporter(gltfImporter), mStatusDialog("AssetLoadStatus") {}

void AssetBrowser::render(liquid::AssetManager &assetManager,
                          IconRegistry &iconRegistry) {
  constexpr uint32_t ITEM_WIDTH = 80;
  constexpr uint32_t ITEM_HEIGHT = 100;
  const ImVec2 ICON_SIZE(80.0f, 80.0f);
  constexpr uint32_t TEXT_WIDTH = ITEM_WIDTH - 8;

  if (mDirectoryChanged) {
    if (mCurrentDirectory.empty()) {
      mCurrentDirectory = assetManager.getAssetsPath();
    }
    mEntries.clear();
    for (auto &dirEntry :
         std::filesystem::directory_iterator(mCurrentDirectory)) {
      Entry entry;
      entry.path = dirEntry.path();
      const auto &pair = assetManager.getRegistry().getAssetByPath(entry.path);
      entry.isDirectory = dirEntry.is_directory();
      entry.assetType = pair.first;
      entry.asset = pair.second;
      entry.icon = entry.isDirectory ? EditorIcon::Directory
                                     : getIconFromAssetType(entry.assetType);
      entry.clippedName = entry.path.filename().stem().string();

      if (ImGui::CalcTextSize(entry.clippedName.c_str()).x > TEXT_WIDTH) {
        liquid::String ellipsis = "..";

        bool changed = false;
        while (ImGui::CalcTextSize((entry.clippedName + ellipsis).c_str()).x >
               TEXT_WIDTH) {
          entry.clippedName.pop_back();
        }

        entry.clippedName += ellipsis;
      }

      mEntries.push_back(entry);
    }

    mDirectoryChanged = false;
  }

  if (ImGui::Begin("Asset Browser")) {
    if (ImGui::BeginPopupContextWindow()) {
      if (ImGui::MenuItem("Import GLTF")) {
        handleGLTFImport();
      }
      ImGui::EndPopup();
    }

    const auto &size = ImGui::GetContentRegionAvail();
    auto itemsPerRow = static_cast<int32_t>(size.x / ITEM_WIDTH);

    auto relativePath = std::filesystem::relative(mCurrentDirectory,
                                                  assetManager.getAssetsPath());

    if (mCurrentDirectory != assetManager.getAssetsPath()) {
      if (ImGui::Button("Back")) {
        mCurrentDirectory = mCurrentDirectory.parent_path();
        mDirectoryChanged = true;
      }
      ImGui::SameLine();
    }
    ImGui::Text("%s", relativePath.string().c_str());

    if (ImGui::BeginTable("CurrentDir", itemsPerRow,
                          ImGuiTableFlags_NoPadInnerX)) {
      for (size_t i = 0; i < mEntries.size(); ++i) {
        const auto &entry = mEntries.at(i);
        auto colIndex = (i % itemsPerRow);
        if (colIndex == 0) {
          ImGui::TableNextRow(ImGuiTableRowFlags_None, ITEM_HEIGHT * 1.0f);
        }

        const auto &filename = entry.path.filename();

        ImGui::TableNextColumn();

        liquid::String id = "###" + filename.string();
        if (ImGui::Selectable(id.c_str(), mSelected == i,
                              ImGuiSelectableFlags_AllowDoubleClick,
                              ImVec2(ITEM_WIDTH, ITEM_HEIGHT))) {
          // Select when item is clicked
          mSelected = i;

          // Double click opens the file/directory
          if (ImGui::IsMouseDoubleClicked(0)) {
            if (entry.isDirectory) {
              mCurrentDirectory = entry.path;
              mDirectoryChanged = true;
            } else if (mOnItemOpenHandler) {
              mOnItemOpenHandler(entry.assetType, entry.asset);
            }
          }
        }

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ITEM_HEIGHT);

        ImguiImage(iconRegistry.getIcon(entry.icon), ICON_SIZE);

        ImGui::Text("%s", entry.clippedName.c_str());
      }
      ImGui::TableNextRow();
    }
    ImGui::EndTable();
  }

  ImGui::End();

  mStatusDialog.render();
}

void AssetBrowser::setOnItemOpenHandler(
    const std::function<void(liquid::AssetType, uint32_t)> &itemOpenHandler) {
  mOnItemOpenHandler = itemOpenHandler;
}

void AssetBrowser::reload() { mDirectoryChanged = true; }

void AssetBrowser::handleGLTFImport() {
  auto filePath = mFileDialog.getFilePathFromDialog({"gltf"});
  if (filePath.empty())
    return;

  auto res = mGltfImporter.loadFromFile(filePath, mCurrentDirectory);

  if (res.hasError()) {
    mStatusDialog.setTitle("GLTF import Error");
    mStatusDialog.setMessages({res.getError()});
    mStatusDialog.show();

    return;
  }

  if (res.hasWarnings()) {
    mStatusDialog.setTitle("GLTF Import: Warnings");
    mStatusDialog.setMessages(res.getWarnings());
    mStatusDialog.show();
  }

  reload();
}

} // namespace liquidator