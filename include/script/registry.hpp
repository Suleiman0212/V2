#pragma once

#include <functional>
#include <initializer_list>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "script/cell.hpp"

class ScriptVm;
using ScriptNativeFnPtr = std::function<ScriptCell(ScriptVm &, std::span<const ScriptCell> params)>;

struct ScriptNativeFn {
  std::string name;
  ScriptCellType return_type;
  std::vector<ScriptCellType> param_types;
  ScriptNativeFnPtr ptr;
};

class ScriptRegistry {
public:
  void add_native_fn(std::string_view name, ScriptCellType return_type, std::initializer_list<ScriptCellType> param_types, ScriptNativeFnPtr fn);

private:
  std::optional<size_t> lookup_native_fn(std::string_view name) const;

  std::vector<ScriptNativeFn> native_fns;

  friend class Parser;
};