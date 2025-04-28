#pragma once

#include <glm/glm.hpp>
#include <string>

struct GLFWwindow;

class Window {
public:
  Window();
  ~Window();
 
  bool init(glm::uvec2 size, const std::string &title);

  bool poll_events();
private:
  GLFWwindow *window = nullptr;
};
