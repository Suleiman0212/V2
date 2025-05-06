#pragma once

#include <functional>
#include <glm/glm.hpp>
#include <string>

using WindowFramebufferCallback = std::function<void(int width, int height)>;

struct GLFWwindow;
class Window {
public:
  Window(const std::string &title, glm::uvec2 size);
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
