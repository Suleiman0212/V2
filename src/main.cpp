#include "include/log.hpp"
#include "include/quad.hpp"
#include "include/render.hpp"
#include "include/window.hpp"
#include "script/cell.hpp"
#include "script/registry.hpp"
#include "script/script.hpp"
#include "script/vm.hpp"
#include <chrono>
#include <span>

const glm::vec4 background_color(0.4f, 0.3f, 0.5f, 1.0f);

ScriptCell script_print_string(ScriptVm &vm, std::span<const ScriptCell> params) {
  trace::debug(vm.get_string(params[0]));
  return 0.0;
}

ScriptCell script_print_number(ScriptVm &vm, std::span<const ScriptCell> params) {
  trace::debug(std::to_string(params[0]));
  return 0.0;
}

static std::chrono::high_resolution_clock::time_point script_end_wait_time;
ScriptCell script_wait(ScriptVm &vm, std::span<const ScriptCell> params) {
  using namespace std::chrono;
  script_end_wait_time = high_resolution_clock::now() + duration_cast<seconds>(duration<double>(params[0]));
  vm.yield();
  return 0.0;
}

int main() {
  Window window("V2", 900, 900);
  Renderer renderer(window);

  ScriptRegistry registry;
  registry.add_native_fn("print_string", ScriptCellType::Void, {ScriptCellType::String}, script_print_string);
  registry.add_native_fn("print_number", ScriptCellType::Void, {ScriptCellType::Number}, script_print_number);
  registry.add_native_fn("wait", ScriptCellType::Void, {ScriptCellType::Number}, script_wait);

  auto script = Script::compile_file("script.v2s", registry);
  if (!script) {
    trace::error("go fix up script.v2s");
    return -1;
  }

  ScriptVm vm(*script);
  vm.call(*vm.lookup_fn("main"));

  trace::info("Window created, Renderer created.");

  Texture template_texture("textures/template.jpg");
  Quad template_quad(glm::vec2(300.0f, 300.0f), glm::vec2(300.0f, 300.0f),
                     template_texture);
  template_quad.origin = glm::vec2(150, 150);

  while (window.running()) {
    if (vm.get_status() != ScriptVmStatus::Stopped) {
      if (std::chrono::high_resolution_clock::now() >= script_end_wait_time) {
        vm.resume();
      }
    }

    template_quad.angle += 1;
    window.clear(background_color);
    renderer.draw_quad(template_quad);
  }
  trace::info("Window closed.");
  return 0;
}
