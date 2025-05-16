#include "log.hpp"
#include "random.hpp"
#include "script/cell.hpp"
#include "script/registry.hpp"
#include "script/vm.hpp"
#include <cmath>
#include <glm/common.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/scalar_reciprocal.hpp>
#include <glm/trigonometric.hpp>
#include <span>
#include <string>
#include <string_view>

namespace {
using UnaryMathFn = ScriptCell (*)(ScriptCell);
template <UnaryMathFn F>
void add_unary_math_fn(ScriptRegistry &registry, std::string_view name) {
  registry.add_native_fn(
      name, ScriptCellType::Number, {ScriptCellType::Number},
      [](auto, std::span<const ScriptCell> params) { return F(params[0]); });
}

using BinaryMathFn = ScriptCell (*)(ScriptCell, ScriptCell);
template <BinaryMathFn F>
void add_binary_math_fn(ScriptRegistry &registry, std::string_view name) {
  registry.add_native_fn(name, ScriptCellType::Number,
                         {ScriptCellType::Number, ScriptCellType::Number},
                         [](auto, std::span<const ScriptCell> params) {
                           return F(params[0], params[1]);
                         });
}

using TernaryMathFn = ScriptCell (*)(ScriptCell, ScriptCell, ScriptCell);
template <TernaryMathFn F>
void add_ternary_math_fn(ScriptRegistry &registry, std::string_view name) {
  registry.add_native_fn(
      name, ScriptCellType::Number,
      {ScriptCellType::Number, ScriptCellType::Number, ScriptCellType::Number},
      [](auto, std::span<const ScriptCell> params) {
        return F(params[0], params[1], params[2]);
      });
}
} // namespace

void ScriptRegistry::load_std_library() {
  add_const("NAN", NAN);
  add_const("INF", INFINITY);
  add_const("PI", glm::pi<ScriptCell>());

  add_unary_math_fn<std::abs>(*this, "abs");
  add_unary_math_fn<std::floor>(*this, "floor");
  add_unary_math_fn<std::ceil>(*this, "ceil");
  add_unary_math_fn<std::round>(*this, "round");
  add_unary_math_fn<glm::radians>(*this, "rad");
  add_unary_math_fn<glm::degrees>(*this, "deg");
  add_unary_math_fn<glm::sin>(*this, "sin");
  add_unary_math_fn<glm::cos>(*this, "cos");
  add_unary_math_fn<glm::tan>(*this, "tan");
  add_unary_math_fn<glm::cot>(*this, "cot");
  add_unary_math_fn<glm::asin>(*this, "asin");
  add_unary_math_fn<glm::acos>(*this, "acos");
  add_unary_math_fn<glm::atan>(*this, "atan");
  add_unary_math_fn<glm::acot>(*this, "acot");

  add_binary_math_fn<std::atan2>(*this, "atan2");

  add_ternary_math_fn<glm::clamp>(*this, "clamp");
  add_ternary_math_fn<glm::mix>(*this, "lerp");

  add_native_fn("print_int", ScriptCellType::Void, {ScriptCellType::Number},
                [](auto, std::span<const ScriptCell> params) {
                  trace::debug(std::to_string((int)params[0]));
                  return 0.0;
                });

  add_native_fn("print_float", ScriptCellType::Void, {ScriptCellType::Number},
                [](auto, std::span<const ScriptCell> params) {
                  trace::debug(std::to_string(params[0]));
                  return 0.0;
                });

  add_native_fn("print_string", ScriptCellType::Void, {ScriptCellType::String},
                [](ScriptVm &vm, std::span<const ScriptCell> params) {
                  trace::debug(vm.get_string(params[0]));
                  return 0.0;
                });

  add_native_fn("rand_int_range", ScriptCellType::Number,
                {ScriptCellType::Number, ScriptCellType::Number},
                [](ScriptVm &, std::span<const ScriptCell> params) {
                  return rand_int_range(params[0], params[1]);
                });

  add_native_fn("rand_float_range", ScriptCellType::Number,
                {ScriptCellType::Number, ScriptCellType::Number},
                [](ScriptVm &, std::span<const ScriptCell> params) {
                  return rand_float_range(params[0], params[1]);
                });
}
