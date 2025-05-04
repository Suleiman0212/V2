#pragma once

#include <initializer_list>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include "script/ast.hpp"
#include "script/cell.hpp"
#include "script/lexer.hpp"
#include "script/registry.hpp"
#include "script/script.hpp"

class Parser {
public:
  static std::optional<Script> parse(std::span<const Token> tokens, const ScriptRegistry &registry);

private:
  struct Var {
    std::string name;
    ScriptCellType type;
    size_t depth; // 1 is also used for function parameters
  };

  enum class Symbol {
    None,
    Var,
    Fn,
    NativeFn,
  };

  Parser(std::span<const Token> tokens, Script &script);

  bool is_eof();
  const Token &peek();
  const Token &peek_prev();
  const Token &peek_next();
  bool check(TokenType type);

  const Token &next();
  bool match(TokenType type);
  bool match(std::initializer_list<TokenType> types);
  const Token *expect(TokenType type);

  Symbol lookup_symbol(std::string_view name, size_t &idx);
  std::string_view symbol_name(Symbol sym);
  std::string_view cell_type_name(ScriptCellType type);
  std::string_view binary_op_name(BinaryOp op);

  template<class T, class... Args>
  AstNodePtr new_node(Args &&...params) {
    return std::make_unique<T>(std::forward<Args>(params)...);
  }

  AstNodePtr fn_ref(bool native, size_t idx);
  AstNodePtr primary();

  AstNodePtr binary(BinaryOp op, AstNodePtr &&lhs, AstNodePtr &&rhs, int line = 0, int col = 0);
  AstNodePtr factor();
  AstNodePtr term();
  AstNodePtr test();
  AstNodePtr expr();

  AstNodePtr var_decl();
  AstNodePtr block();
  AstNodePtr statement();

  void fn_decl();
  void top_level();

  bool has_error();
  void throw_error(std::string_view msg, int line = 0, int col = 0);

  std::span<const Token> tokens;
  size_t token_idx = 0;

  Script &script;
  size_t scope_depth = 0;
  std::vector<Var> vars;

  bool error_flag = false;
};