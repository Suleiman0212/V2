#pragma once

#include "cell.hpp"
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

enum class TokenType {
  Eof,

  LeftParen,
  RightParen,
  LeftBrace,
  RightBrace,

  Not,
  Plus,
  Minus,
  Star,
  Slash,
  Percent,

  Eq,
  NotEq,
  Less,
  LessEq,
  Greater,
  GreaterEq,

  AndAnd,
  PipePipe,

  Assign,
  PlusAssign,
  MinusAssign,
  StarAssign,
  SlashAssign,
  PercentAssign,

  Comma,
  Colon,
  Semicolon,

  Identifier, // string value
  Const,
  Else,
  False,
  Fn,
  Let,
  If,
  True,
  While,

  NumberLiteral, // double value
  StringLiteral, // string value
  CellType,      // cell type value
};

struct Token {
  static std::string_view type_name(TokenType type);

  std::string_view type_name() const { return type_name(type); }
  double as_double() const { return std::get<double>(value); }
  const std::string &as_string() const { return std::get<std::string>(value); }
  ScriptCellType as_cell_type() const {
    return std::get<ScriptCellType>(value);
  }

  TokenType type;
  std::variant<std::monostate, double, std::string, ScriptCellType> value;
  int line, col;
};

class Lexer {
public:
  static std::optional<std::vector<Token>> tokenize(std::string_view source);

private:
  Lexer(std::string_view source);

  std::optional<Token> next_token();
  std::optional<Token> read_number();
  std::optional<Token> read_string();
  Token read_identifier();
  std::optional<Token> read_cell_type(std::string_view identifier);

  Token emit(TokenType type);
  Token emit(TokenType type, double value);
  Token emit(TokenType type, std::string_view value);
  Token emit(TokenType type, ScriptCellType value);

  bool is_eof();
  char peek();
  char peek_prev();
  char peek_next();

  char next();
  bool match(char ch);

  bool has_error();
  void throw_error(std::string_view msg);

  std::string_view source;
  size_t pos = 0;
  int line = 1, col = 1;

  size_t token_pos;
  int token_line, token_col;

  bool error_flag = false;
};