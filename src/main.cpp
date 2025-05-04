#include "include/log.hpp"
#include "include/quad.hpp"
#include "include/render.hpp"
#include "include/window.hpp"
#include "script/cell.hpp"
#include "script/registry.hpp"
#include "script/script.hpp"
#include <span>

const glm::vec4 background_color(0.4f, 0.3f, 0.5f, 1.0f);

ScriptCell script_print_string(ScriptVm &vm, std::span<const ScriptCell> params) {
  trace::debug("haiiii");
  return 0.0;
}

int main() {
  Window window("V2", 900, 900);
  Renderer renderer(window);

  ScriptRegistry registry;
  registry.add_native_fn("print_string", ScriptCellType::Void, {ScriptCellType::String}, script_print_string);
  
  auto script = Script::compile_file("script.v2s", registry);

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
