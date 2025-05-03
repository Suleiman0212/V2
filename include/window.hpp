#pragma once
#include <functional>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>

using WindowFramebufferCallback = std::function<void(int width, int height)>;

class Window {
public:
  Window(std::string title, uint32_t width, uint32_t height);
  ~Window();

  bool running();
  void clear(glm::vec4 color) const;
  bool is_key_down(int key);

  void get_framebuffer_size(int &width, int &height) const;

  WindowFramebufferCallback framebuffer_callback = nullptr;

private:
  GLFWwindow *window;

  void events();
};
