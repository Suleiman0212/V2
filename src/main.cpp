#include "include/log.hpp"
#include "include/quad.hpp"
#include "include/render.hpp"
#include "include/window.hpp"

const glm::vec4 BACKGROUND_COLOR(153.0f / 250, 59.0f / 250, 80.0f / 250, 1.0f);
const float QUAD_SPEED = 95;

int main() {
  Window window("V2", 1280, 720);
  Renderer renderer(window);

  trace::info("Window created, Renderer created.");

  Texture background_texture("");
  Quad background_quad(glm::vec2(0, 0), glm::vec2(1280, 720),
                       background_texture);

  Texture eye_texture(
      "assets/textures/eye_of_providence/eye_of_providence.png");
  Quad eye_quad(glm::vec2(1280 / 2 - 32 * 3, 64), glm::vec2(64 * 3, 64 * 3),
                eye_texture);

  Texture jesus_texture("assets/textures/jesus/jesus.png");
  Quad jesus_quad(glm::vec2(1280 / 2 - 32 * 2, 64 + 500),
                  glm::vec2(64 * 2, 64 * 2), jesus_texture);

  while (window.running()) {
    eye_quad.angle += 1;

    glm::vec2 move_dir{};

    if (window.is_key_down(GLFW_KEY_W)) {
      move_dir.y += -1;
    }

    if (window.is_key_down(GLFW_KEY_S)) {
      move_dir.y += 1;
    }

    if (window.is_key_down(GLFW_KEY_A)) {
      move_dir.x += -1;
    }

    if (window.is_key_down(GLFW_KEY_D)) {
      move_dir.x += 1;
    }

    if (move_dir != glm::vec2())
      jesus_quad.pos += glm::normalize(move_dir * QUAD_SPEED);

    window.clear(BACKGROUND_COLOR);
    renderer.draw_quads({background_quad, eye_quad, jesus_quad});
  }
  trace::info("Window closed.");
  return 0;
}
