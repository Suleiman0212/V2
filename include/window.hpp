#pragma once

#include "math/vec2.hpp"
#include "math/vec4.hpp"
#include <functional>
#include <string>

using WindowFramebufferCallback = std::function<void(int width, int height)>;

struct GLFWwindow;
class Window {
public:
  Window(const std::string &title, Vec2u size);
  ~Window();

  bool is_running() const;

  void clear(Vec4f color) const;
  void present() const;

  bool is_key_down(int key);

  void get_framebuffer_size(int &width, int &height) const;

  WindowFramebufferCallback framebuffer_callback = nullptr;

private:
  GLFWwindow *window = nullptr;
};
