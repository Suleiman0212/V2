#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "script/ast.hpp"
#include "script/cell.hpp"
#include "script/registry.hpp"

struct ScriptFn {
  std::string name;
  std::vector<ScriptCellType> param_types;
  AstNodePtr body;
};

class Script {
public:
  static std::optional<Script> compile(std::string_view source, const ScriptRegistry &registry);
  static std::optional<Script> compile_file(const std::string &filename, const ScriptRegistry &registry);

  std::optional<size_t> lookup_fn(std::string_view name) const;
  std::optional<size_t> lookup_string(std::string_view str) const;

private:
  Script(const ScriptRegistry &registry);

  size_t add_string(std::string_view str);

  std::vector<ScriptFn> fns;
  std::vector<std::string> str_table;
  ScriptRegistry registry;

  friend class Parser;
};