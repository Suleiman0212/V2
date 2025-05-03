#include "include/log.hpp"
#include "include/quad.hpp"
#include "include/render.hpp"
#include "include/window.hpp"
#include "script/lexer.hpp"

const glm::vec4 background_color(0.4f, 0.3f, 0.5f, 1.0f);

int main() {
  Window window("V2", 900, 900);
  Renderer renderer(window);

  auto tokens = Lexer::tokenize_file("script.v2s");

  trace::info("Window created, Renderer created.");

  Texture template_texture("textures/template.jpg");
  Quad template_quad(glm::vec2(300.0f, 300.0f), glm::vec2(300.0f, 300.0f),
                     template_texture);
  template_quad.origin = glm::vec2(150, 150);

  while (window.running()) {
    template_quad.angle += 1;
    window.clear(background_color);
    renderer.draw_quad(template_quad);
  }
  trace::info("Window closed.");
  return 0;
}
