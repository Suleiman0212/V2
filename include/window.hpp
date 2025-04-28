#pragma once

#include <glm/glm.hpp>
#include <string>

struct GLFWwindow;

class Window {
public:
  ~Window();
 
  bool init(glm::uvec2 size, const std::string &title);

  bool poll_events();
  void present();
private:
  void deinit();

  GLFWwindow *window = nullptr;
};
