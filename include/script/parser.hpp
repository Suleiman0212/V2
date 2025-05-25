#pragma once

#include "ast.hpp"
#include "cell.hpp"
#include "lexer.hpp"
#include "registry.hpp"
#include "script.hpp"
#include <initializer_list>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

class Parser {
public:
  static std::optional<Script> parse(std::span<const Token> tokens,
                                     const ScriptRegistry &registry);

private:
  struct Const {
    std::string name;
    ScriptCell value;
    size_t depth; // 0 = toplevel/registry consts
  };

  struct Var {
    std::string name;
    ScriptCellType type;
    size_t depth; // 1 is also used for function parameters
  };

  enum class Symbol {
    None,
    Const,
    Var,
    Fn,
    NativeFn,
    DesStruct,
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

  std::optional<size_t> lookup_const(std::string_view name);
  std::optional<size_t> lookup_var(std::string_view name);
  Symbol lookup_symbol(std::string_view name, size_t &idx);
  void check_symbol_redef(std::string_view name, int line = 0, int col = 0);

  std::string_view symbol_name(Symbol sym);
  std::string_view cell_type_name(ScriptCellType type);
  std::string_view unary_op_name(UnaryOp op);
  std::string_view binary_op_name(BinaryOp op);

  template <class T, class... Args> AstNodePtr new_node(Args &&...params) {
    return std::make_unique<T>(std::forward<Args>(params)...);
  }

  AstNodePtr unary(UnaryOp op, AstNodePtr &&value, int line = 0, int col = 0);
  AstNodePtr des_struct(size_t idx);
  AstNodePtr fn_ref(bool native, size_t idx);
  AstNodePtr primary();

  AstNodePtr binary(BinaryOp op, AstNodePtr &&lhs, AstNodePtr &&rhs,
                    int line = 0, int col = 0);
  BinaryOp token_type_to_binary_op(TokenType type);
  AstNodePtr factor();
  AstNodePtr term();
  AstNodePtr rel_test();
  AstNodePtr abs_test();
  AstNodePtr logic_and();
  AstNodePtr logic_or();
  AstNodePtr expr();

  AstNodePtr while_statement();
  AstNodePtr if_statement();
  AstNodePtr var_decl();
  AstNodePtr block();
  AstNodePtr statement();

  void const_decl();
  void fn_decl();
  void top_level();

  void fold_consts(AstNodePtr &node);

  bool has_error();
  void throw_error(std::string_view msg, int line = 0, int col = 0);

  std::span<const Token> tokens;
  size_t token_idx = 0;

  Script &script;
  size_t scope_depth = 0;
  std::vector<Const> consts;
  std::vector<Var> vars;

  bool error_flag = false;
};