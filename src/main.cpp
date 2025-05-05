#include "log.hpp"
#include "quad.hpp"
#include "render.hpp"
#include "util.hpp"
#include "window.hpp"
#include "script/cell.hpp"
#include "script/registry.hpp"
#include "script/script.hpp"
#include "script/vm.hpp"
#include <chrono>
#include <format>
#include <span>
#include <string_view>

const glm::vec4 BACKGROUND_COLOR(153.0f / 250, 59.0f / 250, 80.0f / 250, 1.0f);
const float QUAD_SPEED = 95;

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

struct Logger {
  Logger(uint32_t id): id(id) {}

  void print_string(std::string_view str) {
    trace::debug(std::format("{}: {} [{}]", prefix, str, stamp));
  }

  uint32_t id;
  std::string prefix;
  double stamp = 0.0;
};

static std::vector<Logger> loggers;
static uint32_t next_logger_id = 1;

ScriptCell script_logger_new(ScriptVm &vm, std::span<const ScriptCell> params) {
  uint32_t id = next_logger_id++;
  loggers.emplace_back(id);
  return id;
}

std::optional<size_t> get_logger(uint32_t id) {
  return util::index_of(loggers.begin(), loggers.end(), [id](const auto &x) {
    return x.id == id;
  });
}

ScriptCell script_logger_destroy(ScriptVm &vm, std::span<const ScriptCell> params) {
  if (auto idx = get_logger(params[0])) {
    loggers.erase(loggers.begin() + *idx);
  }
  return 0.0;
}

ScriptCell script_logger_set_prefix(ScriptVm &vm, std::span<const ScriptCell> params) {
  if (auto idx = get_logger(params[0])) {
    loggers[*idx].prefix = vm.get_string(params[1]);
  }
  return 0.0;
}

ScriptCell script_logger_set_stamp(ScriptVm &vm, std::span<const ScriptCell> params) {
  if (auto idx = get_logger(params[0])) {
    loggers[*idx].stamp = params[1];
  }
  return 0.0;
}

ScriptCell script_logger_print_string(ScriptVm &vm, std::span<const ScriptCell> params) {
  if (auto idx = get_logger(params[0])) {
    loggers[*idx].print_string(vm.get_string(params[1]));
  }
  return 0.0;
}

int main() {
  Window window("V2", 1280, 720);
  Renderer renderer(window);

  ScriptRegistry registry;
  registry.add_native_fn("print_string", ScriptCellType::Void, {ScriptCellType::String}, script_print_string);
  registry.add_native_fn("print_number", ScriptCellType::Void, {ScriptCellType::Number}, script_print_number);
  registry.add_native_fn("wait", ScriptCellType::Void, {ScriptCellType::Number}, script_wait);

  registry.add_native_fn("logger_new", ScriptCellType::Number, {}, script_logger_new);
  registry.add_native_fn("logger_destroy", ScriptCellType::Void, {ScriptCellType::Number}, script_logger_destroy);
  registry.add_native_fn("logger_set_prefix", ScriptCellType::Void, {ScriptCellType::Number, ScriptCellType::String}, script_logger_set_prefix);
  registry.add_native_fn("logger_set_stamp", ScriptCellType::Void, {ScriptCellType::Number, ScriptCellType::Number}, script_logger_set_stamp);
  registry.add_native_fn("logger_print_string", ScriptCellType::Void, {ScriptCellType::Number, ScriptCellType::String}, script_logger_print_string);
  
  registry.add_des_struct("Logger", "logger_new", {
    {"prefix", "logger_set_prefix"},
    {"stamp", "logger_set_stamp"},
  });

  auto script = Script::compile_file("script.v2s", registry);
  if (!script) {
    trace::error("go fix up script.v2s");
    return -1;
  }

  ScriptVm vm(*script);
  vm.call(*vm.lookup_fn("main"));

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
