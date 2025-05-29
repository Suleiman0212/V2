#include "log.hpp"
#include "math/misc.hpp"
#include "random.hpp"
#include "script/cell.hpp"
#include "script/registry.hpp"
#include "script/vm.hpp"
#include <cmath>
#include <format>
#include <span>
#include <string>
#include <string_view>

namespace {
using UnaryMathFn = ScriptCell (*)(ScriptCell);
template <UnaryMathFn F>
void add_unary_math_fn(ScriptRegistry &registry, std::string_view name) {
  registry.add_native_fn(
      std::format("number {}(number) const", name),
      [](auto, ScriptParams params) { return F(params[0]); });
}

using BinaryMathFn = ScriptCell (*)(ScriptCell, ScriptCell);
template <BinaryMathFn F>
void add_binary_math_fn(ScriptRegistry &registry, std::string_view name) {
  registry.add_native_fn(
      std::format("number {}(number, number) const", name),
      [](auto, ScriptParams params) { return F(params[0], params[1]); });
}

using TernaryMathFn = ScriptCell (*)(ScriptCell, ScriptCell, ScriptCell);
template <TernaryMathFn F>
void add_ternary_math_fn(ScriptRegistry &registry, std::string_view name) {
  registry.add_native_fn(
      std::format("number {}(number, number, number) const", name),
      [](auto, ScriptParams params) {
        return F(params[0], params[1], params[2]);
      });
}
} // namespace

void ScriptRegistry::load_std_library() {
  add_const("NAN", NAN);
  add_const("INF", INFINITY);
  add_const("PI", math::PI);

  add_unary_math_fn<std::abs>(*this, "abs");
  add_unary_math_fn<std::floor>(*this, "floor");
  add_unary_math_fn<std::ceil>(*this, "ceil");
  add_unary_math_fn<std::round>(*this, "round");
  add_unary_math_fn<math::rad>(*this, "rad");
  add_unary_math_fn<math::deg>(*this, "deg");
  add_unary_math_fn<std::sin>(*this, "sin");
  add_unary_math_fn<std::cos>(*this, "cos");
  add_unary_math_fn<std::tan>(*this, "tan");
  add_unary_math_fn<std::asin>(*this, "asin");
  add_unary_math_fn<std::acos>(*this, "acos");
  add_unary_math_fn<std::atan>(*this, "atan");

  add_binary_math_fn<std::atan2>(*this, "atan2");

  add_ternary_math_fn<math::clamp>(*this, "clamp");
  add_ternary_math_fn<std::lerp>(*this, "lerp");

  add_native_fn("void print_int(number)", [](auto, ScriptParams params) {
    trace::debug(std::to_string((int)params[0]));
    return 0.0;
  });

  add_native_fn("void print_float(number)", [](auto, ScriptParams params) {
    trace::debug(std::to_string(params[0]));
    return 0.0;
  });

  add_native_fn("void print_string(string)",
                [](ScriptVm &vm, ScriptParams params) {
                  trace::debug(vm.get_string(params[0]));
                  return 0.0;
                });

  add_native_fn("number rand_int_range(number, number)",
                [](auto, ScriptParams params) {
                  return rng::next_int_range(params[0], params[1]);
                });

  add_native_fn("number rand_float_range(number, number)",
                [](auto, ScriptParams params) {
                  return rng::next_float_range(params[0], params[1]);
                });

  add_native_fn("void yield()", [](ScriptVm &vm, auto) {
    vm.yield();
    return 0.0;
  });
}
