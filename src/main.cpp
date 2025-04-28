#include "window.hpp"

int main() {
  Window window;
  window.init(glm::uvec2(640, 480), "HUYNA");

  while (window.poll_events()) {

  }

  return 0;
}
