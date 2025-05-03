#include "script/lexer.hpp"
#include <charconv>
#include <format>
#include <fstream>
#include <memory>
#include <string>
#include "log.hpp"

std::optional<std::vector<Token>> Lexer::tokenize(std::string_view source) {
  Lexer lexer(source);
  std::vector<Token> tokens;
  while (auto token = lexer.next_token()) {
    tokens.emplace_back(std::move(*token));
  }

  if (lexer.has_error()) return std::nullopt;
  return tokens;
}

std::optional<std::vector<Token>> Lexer::tokenize_file(const std::string &filename) {
  std::ifstream stream(filename, std::ios_base::ate);
  if (!stream.is_open()) {
    trace::error(std::format("can't open file \"{}\"", filename));
    return std::nullopt;
  }

  size_t len = stream.tellg();
  stream.seekg(0);
  auto source = std::make_unique<char[]>(len);
  stream.read(source.get(), len);

  return tokenize(std::string_view(source.get(), len));
}

Lexer::Lexer(std::string_view source): source(source) {}

std::optional<Token> Lexer::next_token() {
again:
  token_pos = pos;
  token_line = line;
  token_col = col;

  char ch = next();
  switch (ch) {
    case '\0': return std::nullopt;
    case '(': return emit(TokenType::LeftParen);
    case ')': return emit(TokenType::RightParen);
    case '{': return emit(TokenType::LeftBrace);
    case '}': return emit(TokenType::RightBrace);
    case '!':
      if (match('=')) return emit(TokenType::NotEq);
      return emit(TokenType::Not);
    case '+':
      if (match('=')) return emit(TokenType::PlusAssign);
      return emit(TokenType::Plus);
    case '-':
      if (match('=')) return emit(TokenType::MinusAssign);
      return emit(TokenType::Minus);
    case '*':
      if (match('=')) return emit(TokenType::StarAssign);
      return emit(TokenType::Star);
    case '/':
      if (match('=')) return emit(TokenType::SlashAssign);
      return emit(TokenType::Slash);
    case '=': 
      if (match('=')) return emit(TokenType::Eq);
      return emit(TokenType::Assign);
    case '<':
      if (match('=')) return emit(TokenType::LessEq);
      return emit(TokenType::Less);
    case '>':
      if (match('=')) return emit(TokenType::GreaterEq);
      return emit(TokenType::Greater);
    case ',': return emit(TokenType::Comma);
    case ':': return emit(TokenType::Colon);
    case ';': return emit(TokenType::Semicolon);
    case '"':
      if (auto token = read_string()) return token;
      goto again;
    default: {
      if (isdigit(ch)) {
        if (auto token = read_number()) return token;
      } else if (isalpha(ch) || ch == '_') {
        return read_identifier();
      } else if (!isspace(ch)) {
        throw_error(std::format("unknown token '{}'", ch));
      }

      goto again;
    }
  }
}

std::optional<Token> Lexer::read_number() {
  while (isdigit(peek())) next();
  if (peek() == '.' && isdigit(peek_next())) {
    // decimal
    next();
    while (isdigit(peek())) next();
  }

  double value;
  auto conv_result = std::from_chars(source.data() + token_pos, source.data() + pos, value);
  if (conv_result.ec != std::errc()) {
    throw_error("malformed number");
    return std::nullopt;
  }
  return emit(TokenType::NumberLiteral, value);
}

std::optional<Token> Lexer::read_string() {
  // TODO: escape sequences
  while (!is_eof() && peek() != '\n' && peek() != '\"') next();

  if (peek() != '\"') {
    throw_error("unclosed quotes");
    next();
    return std::nullopt;
  }

  std::string_view str = source.substr(token_pos + 1, pos - token_pos - 1);
  next();
  return emit(TokenType::StringLiteral, str);
}

Token Lexer::read_identifier() {
  while (isalnum(peek()) || peek() == '_') next();

  std::string_view identifier = source.substr(token_pos, pos - token_pos);
  if (identifier == "else") {
    return emit(TokenType::Else);
  } else if (identifier == "fn") {
    return emit(TokenType::Fn);
  } else if (identifier == "let") {
    return emit(TokenType::Let);
  } else if (identifier == "if") {
    return emit(TokenType::If);
  }
  return emit(TokenType::Identifier, identifier);
}

Token Lexer::emit(TokenType type) {
  return Token(type, {}, token_line, token_col);
}

Token Lexer::emit(TokenType type, double value) {
  return Token(type, value, token_line, token_col);
}

Token Lexer::emit(TokenType type, std::string_view value) {
  return Token(type, std::string(value), token_line, token_col);
}

bool Lexer::is_eof() {
  return pos >= source.size();
}

char Lexer::peek() {
  if (is_eof()) return '\0';
  return source[pos];
}

char Lexer::peek_prev() {
  if (pos == 0) return '\0';
  return source[pos - 1];
}

char Lexer::peek_next() {
  if (pos + 1 >= source.size()) return '\0';
  return source[pos + 1];
}

char Lexer::next() {
  if (is_eof()) return '\0';
  char ch = source[pos++];
  if (ch == '\n') {
    line++;
    col = 1;
  } else {
    col++;
  }

  return ch;
}

bool Lexer::match(char ch) {
  if (peek() != ch) return false;
  next();
  return true;
}

bool Lexer::has_error() {
  return error_flag;
}

void Lexer::throw_error(std::string_view msg) {
  trace::error(std::format("({}:{}) {}", token_line, token_col, msg));
  error_flag = true;
}