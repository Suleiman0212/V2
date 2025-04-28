#include "window.hpp"
#include <format>
#include <GLFW/glfw3.h>
#include "log.hpp"

namespace {
  const char *get_glfw_error() {
    const char *error;
    glfwGetError(&error);
    if (error == nullptr) error = "";
    return error;
  }
}

Window::~Window() {
  if (window != nullptr) glfwDestroyWindow(window);
}

bool Window::init(glm::uvec2 size, const std::string &title) {
  if (!glfwInit()) {
    trace::error(std::format("Can't init glfw3: {}", get_glfw_error()));
    return false;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  if (window != nullptr) glfwDestroyWindow(window);
  window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
  if (window == nullptr) {
    trace::error(std::format("Can't create {}x{} window: {}", size.x, size.y, get_glfw_error()));
    return false;
  }

  return true;
}

bool Window::poll_events() {
  glfwPollEvents();
  return !glfwWindowShouldClose(window);
}
