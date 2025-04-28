#include "window.hpp"
#include "gfx.hpp"

int main() {
  Window window;
  window.init(glm::uvec2(640, 480), "v2");
  Window::make_context_current(&window);

  gfx::init();

  while (window.poll_events()) {
    gfx::clear(glm::vec3(1.0f, 0.0f, 0.0f));

    window.present();
  }

  return 0;
}
