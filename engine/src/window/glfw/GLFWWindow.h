#pragma once

#define GLFW_INCLUDE_VULKAN

#include "core/Base.h"
#include <GLFW/glfw3.h>

namespace liquid {

struct SizeObject {
  uint32_t width;
  uint32_t height;
};

/**
 * @brief GLFW Window
 *
 * Manages lifecycle and handles of
 * GLFW window
 */
class GLFWWindow {
public:
  /**
   * @brief Creates window
   *
   * @param title Window title
   * @param width Window width
   * @param height Window height
   */
  GLFWWindow(const String &title, uint32_t width, uint32_t height);

  /**
   * @brief Destroys window
   */
  ~GLFWWindow();

  GLFWWindow(const GLFWWindow &rhs) = delete;
  GLFWWindow(GLFWWindow &&rhs) = delete;
  GLFWWindow &operator=(const GLFWWindow &rhs) = delete;
  GLFWWindow &operator=(GLFWWindow &&rhs) = delete;

  /**
   * @brief Gets GLFW instance
   *
   * @return GLFWwindow Pointer to GLFW window instance
   */
  inline ::GLFWwindow *getInstance() { return windowInstance; }

  /**
   * @brief Gets framebuffer size
   *
   * @return Framebuffer size
   */
  SizeObject getFramebufferSize();

  /**
   * @brief Gets window size
   *
   * @return Framebuffer size
   */
  SizeObject getWindowSize();

  /**
   * @brief Checks is window should close
   *
   * @retval true Window should close
   * @retval false Window should not close
   */
  inline bool shouldClose() { return glfwWindowShouldClose(windowInstance); }

  /**
   * @brief Polls window events
   */
  inline void pollEvents() { glfwPollEvents(); }

  /**
   * @brief Add resize handler
   *
   * @param handler Resize handler
   * @return Resize handler ID
   */
  uint32_t
  addResizeHandler(const std::function<void(uint32_t, uint32_t)> &handler);

  /**
   * @brief Remove resize handler
   *
   * @param handle Handle
   */
  void removeResizeHandler(uint32_t handle);

  /**
   * @brief Add key handler
   *
   * @param handler Key handler
   * @return Key handler ID
   */
  uint32_t addKeyHandler(
      const std::function<void(int key, int scancode, int action, int mods)>
          &handler);

  /**
   * @brief Remove key down handler
   *
   * @param handle Handle
   */
  void removeKeyHandler(uint32_t handle);

private:
  ::GLFWwindow *windowInstance;

  template <class FunctionType>
  using HandlerMap = std::map<uint32_t, std::function<FunctionType>>;

  HandlerMap<void(uint32_t, uint32_t)> resizeHandlers;
  HandlerMap<void(int key, int scancode, int action, int mods)> keyHandlers;
};

} // namespace liquid
