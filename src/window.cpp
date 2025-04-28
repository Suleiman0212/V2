#define GLFW_INCLUDE_NONE
#include "window.hpp"
#include <format>
#include <GLFW/glfw3.h>
#include "log.hpp"

namespace {
  size_t glfw_ref_count = 0;

  const char *get_glfw_error() {
    const char *error;
    glfwGetError(&error);
    if (error == nullptr) error = "";
    return error;
  }
}

void Window::make_context_current(Window *window) {
  glfwMakeContextCurrent((window != nullptr)? window->glfw_handle : nullptr);
}

Window::~Window() {
  deinit();
}

bool Window::init(glm::uvec2 size, const std::string &title) {
  deinit();

  if (!glfwInit()) {
    trace::error(std::format("Can't init glfw3: {}", get_glfw_error()));
    return false;
  }
  glfw_ref_count++;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
  glfwWindowHint(GLFW_RESIZABLE, false);
  
  glfw_handle = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
  if (glfw_handle == nullptr) {
    trace::error(std::format("Can't create {}x{} window: {}", size.x, size.y, get_glfw_error()));
    return false;
  }

  return true;
}

bool Window::poll_events() {
  glfwPollEvents();
  return !glfwWindowShouldClose(glfw_handle);
}

void Window::present() {
  glfwSwapBuffers(glfw_handle);
}

void Window::deinit() {
  if (glfw_handle != nullptr) glfwDestroyWindow(glfw_handle);
  if (glfw_ref_count > 0) {
    glfw_ref_count--;
    if (glfw_ref_count == 0) glfwTerminate();
  }
}
