#pragma once

#include "cell.hpp"
#include <functional>
#include <initializer_list>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

using ScriptParams = std::span<const ScriptCell>;

class ScriptVm;
using ScriptNativeFnPtr =
    std::function<ScriptCell(ScriptVm &, ScriptParams params)>;

struct ScriptNativeFn {
  std::string name;
  ScriptCellType return_type;
  std::vector<ScriptCellType> param_types;
  bool is_const;

  ScriptNativeFnPtr ptr;
};

struct ScriptDesStruct {
  struct Field {
    std::string name;
    size_t setter_idx;
  };

  std::optional<size_t> lookup_field(std::string_view name) const;

  std::string name;
  size_t factory_idx;
  std::vector<Field> fields;
};

struct ScriptDesField {
  std::string_view name;
  std::string_view setter_name;
};

struct ScriptConst {
  std::string name;
  ScriptCell value;
};

class ScriptRegistry {
public:
  void add_const(std::string_view name, ScriptCell value);
  void add_native_fn(std::string_view prototype, ScriptNativeFnPtr ptr);
  void add_des_struct(std::string_view name, std::string_view factory_name,
                      std::initializer_list<ScriptDesField> fields);

  void load_std_library();

private:
  void check_symbol_redef(std::string_view name);

  std::optional<size_t> lookup_const(std::string_view name) const;
  std::optional<size_t> lookup_native_fn(std::string_view name) const;
  std::optional<size_t> lookup_des_struct(std::string_view name) const;

  std::vector<ScriptConst> consts;
  std::vector<ScriptNativeFn> native_fns;
  std::vector<ScriptDesStruct> des_structs;

  friend class Parser;
  friend class ScriptVm;
};