#include "script/registry.hpp"
#include <initializer_list>
#include <stdexcept>
#include "script/cell.hpp"
#include "util.hpp"

void ScriptRegistry::add_native_fn(std::string_view name, ScriptCellType return_type, std::initializer_list<ScriptCellType> param_types, ScriptNativeFnPtr fn) {
  if (lookup_native_fn(name)) throw std::runtime_error("symbol already exists");
  native_fns.emplace_back(std::string(name), return_type, param_types, fn);
}

std::optional<size_t> ScriptRegistry::lookup_native_fn(std::string_view name) const {
  return util::index_of(native_fns.begin(), native_fns.end(), [name](const auto &x) {
    return x.name == name;
  });
}