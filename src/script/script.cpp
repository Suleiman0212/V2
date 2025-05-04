#include "script/script.hpp"
#include <format>
#include <fstream>
#include "log.hpp"
#include "script/lexer.hpp"
#include "script/parser.hpp"
#include "script/registry.hpp"
#include "util.hpp"

std::optional<Script> Script::compile(std::string_view source, const ScriptRegistry &registry) {
  auto tokens = Lexer::tokenize(source);
  if (!tokens) return std::nullopt;
  return Parser::parse(*tokens, registry);
}

std::optional<Script> Script::compile_file(const std::string &filename, const ScriptRegistry &registry) {
  std::ifstream stream(filename, std::ios_base::ate);
  if (!stream.is_open()) {
    trace::error(std::format("can't open file \"{}\"", filename));
    return std::nullopt;
  }

  size_t len = stream.tellg();
  stream.seekg(0);
  auto source = std::make_unique<char[]>(len);
  stream.read(source.get(), len);

  return compile(std::string_view(source.get(), len), registry);
}

Script::Script(const ScriptRegistry &registry): registry(registry) {}

size_t Script::add_string(std::string_view str) {
  if (auto idx = lookup_string(str)) {
    // reuse
    return *idx;
  } else {
    str_table.emplace_back(std::string(str));
    return str_table.size() - 1;
  }
}

std::optional<size_t> Script::lookup_fn(std::string_view name) const {
  return util::index_of(fns.begin(), fns.end(), [name](const auto &x) {
    return x.name == name;
  });
}

std::optional<size_t> Script::lookup_string(std::string_view str) const {
  return util::index_of(str_table.begin(), str_table.end(), [str](const auto &x) {
    return x == str;
  });
}