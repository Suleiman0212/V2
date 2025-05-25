#include "script/registry.hpp"
#include <format>
#include <initializer_list>
#include <optional>
#include <stdexcept>
#include <string_view>
#include "script/cell.hpp"
#include "util.hpp"

std::optional<size_t> ScriptDesStruct::lookup_field(std::string_view name) const {
  return util::index_of(fields.begin(), fields.end(), [name](const auto &x) {
    return x.name == name;
  });
}

void ScriptRegistry::add_const(std::string_view name, ScriptCell value) {
  if (is_symbol_defined(name)) throw std::runtime_error("symbol is already defined");
  consts.emplace_back(std::string(name), value);
}

void ScriptRegistry::add_native_fn(std::string_view name, ScriptCellType return_type, std::initializer_list<ScriptCellType> param_types, ScriptNativeFnPtr fn) {
  if (is_symbol_defined(name)) throw std::runtime_error("symbol is already defined");
  native_fns.emplace_back(std::string(name), return_type, param_types, fn);
}

void ScriptRegistry::add_des_struct(std::string_view name, std::string_view factory_name, std::initializer_list<ScriptDesField> fields) {
  if (is_symbol_defined(name)) throw std::runtime_error("symbol is already defined");

  ScriptDesStruct des_struct;
  des_struct.name = std::string(name);
  // factory
  if (auto factory_idx = lookup_native_fn(factory_name)) {
    const auto &factory_fn = native_fns[*factory_idx];
    if (factory_fn.return_type != ScriptCellType::Number || !factory_fn.param_types.empty()) {
      throw std::runtime_error("factory must return a number and have no parameters");
    }

    des_struct.factory_idx = *factory_idx;
  } else {
    throw std::runtime_error("factory not found");
  }

  // fields
  for (const auto &field: fields) {
    if (auto setter_idx = lookup_native_fn(field.setter_name)) {
      const auto &setter_fn = native_fns[*setter_idx];
      if (setter_fn.return_type != ScriptCellType::Void || setter_fn.param_types.size() != 2) {
        throw std::runtime_error(std::format("setter \"{}\" must not return anything and have 2 parameters", field.setter_name));
      }
      if (setter_fn.param_types[0] != ScriptCellType::Number) {
        throw std::runtime_error(std::format("first parameter of setter \"{}\" must be a number", field.setter_name));
      }

      des_struct.fields.emplace_back(std::string(field.name), *setter_idx);
    } else {
      throw std::runtime_error(std::format("setter \"{}\" for field \"{}\" not found", field.setter_name, field.name));
    }
  }

  des_structs.emplace_back(std::move(des_struct));
}

bool ScriptRegistry::is_symbol_defined(std::string_view name) const {
  return lookup_const(name) || lookup_native_fn(name) || lookup_des_struct(name);
}

std::optional<size_t> ScriptRegistry::lookup_const(std::string_view name) const {
  return util::index_of(consts.begin(), consts.end(), [name](const auto &x) {
    return x.name == name;
  });
}

std::optional<size_t> ScriptRegistry::lookup_native_fn(std::string_view name) const {
  return util::index_of(native_fns.begin(), native_fns.end(), [name](const auto &x) {
    return x.name == name;
  });
}

std::optional<size_t> ScriptRegistry::lookup_des_struct(std::string_view name) const {
  return util::index_of(des_structs.begin(), des_structs.end(), [name](const auto &x) {
    return x.name == name;
  });
}