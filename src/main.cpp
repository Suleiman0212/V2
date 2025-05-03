#include "window.hpp"
#include "script/lexer.hpp"

int main() {
  Window window;
  window.init(glm::uvec2(640, 480), "v2");

  auto tokens = Lexer::tokenize("%");

  while (window.poll_events()) {
    window.present();
  }

  return 0;
}
