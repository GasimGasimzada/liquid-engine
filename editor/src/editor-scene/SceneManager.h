#pragma once

#include "liquid/scene/Scene.h"
#include "EditorCamera.h"
#include "EditorGrid.h"

#include "EntityManager.h"

namespace liquidator {

class SceneManager {
public:
  /**
   * @brief Create scene manager
   *
   * @param entityContext Entity context
   * @param editorCamera Editor camera
   * @param editorGrid Editor grid
   * @param scenePath Scene path
   * @param entityManager Entity manager
   */
  SceneManager(liquid::EntityContext &entityContext, EditorCamera &editorCamera,
               EditorGrid &editorGrid, EntityManager &entityManager);

  SceneManager(const SceneManager &) = delete;
  SceneManager(SceneManager &&) = delete;
  SceneManager &operator=(const SceneManager &) = delete;
  SceneManager &operator=(SceneManager &&) = delete;
  ~SceneManager() = default;

  /**
   * @brief Save editor state to a file
   *
   * @param path Path to editor state file
   */
  void saveEditorState(const std::filesystem::path &path);

  /**
   * @brief Load editor state from file
   *
   * @param path Path to editor state file
   */
  void loadEditorState(const std::filesystem::path &path);

  /**
   * @brief Get active scene
   *
   * @return Active scene
   */
  inline liquid::Scene *getActiveScene() { return mActiveScene; }

  /**
   * @brief Get editor camera
   *
   * @return Editor camera
   */
  inline EditorCamera &getEditorCamera() { return mEditorCamera; }

  /**
   * @brief Get editor grid
   *
   * @return Editor grid
   */
  inline EditorGrid &getEditorGrid() { return mEditorGrid; }

  /**
   * @brief Get entity manager
   *
   * @return Entity manager
   */
  inline EntityManager &getEntityManager() { return mEntityManager; }

  /**
   * @brief Creates a new scene and sets it as active
   */
  void createNewScene();

  /**
   * @brief Load or create scene
   */
  void loadOrCreateScene();

  /**
   * @brief Move camera to entity location
   *
   * @param entity Entity
   */
  void moveCameraToEntity(liquid::Entity entity);

private:
  liquid::Scene *mActiveScene = nullptr;
  liquid::EntityContext &mEntityContext;
  EditorCamera &mEditorCamera;
  EditorGrid &mEditorGrid;
  std::filesystem::path mScenePath;
  EntityManager &mEntityManager;
  uint32_t mLastId = 1;
};

} // namespace liquidator
