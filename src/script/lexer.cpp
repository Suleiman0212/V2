#include "script/lexer.hpp"
#include <charconv>
#include <format>
#include <string>
#include "log.hpp"

std::string_view Token::type_name(TokenType type) {
  switch (type) {
    case TokenType::Eof: return "<eof>";

    case TokenType::LeftParen: return "'('";
    case TokenType::RightParen: return "')'";
    case TokenType::LeftBrace: return "'{'";
    case TokenType::RightBrace: return "'}'";

    case TokenType::Not: return "'!'";
    case TokenType::Plus: return "'+'";
    case TokenType::Minus: return "'-'";
    case TokenType::Star: return "'*'";
    case TokenType::Slash: return "'/'";

    case TokenType::Eq: return "'=='";
    case TokenType::NotEq: return "'!='";
    case TokenType::Less: return "'<'";
    case TokenType::LessEq: return "'<='";
    case TokenType::Greater: return "'>'";
    case TokenType::GreaterEq: return "'>='";

    case TokenType::Assign: return "'='";
    case TokenType::PlusAssign: return "'+='";
    case TokenType::MinusAssign: return "'-='";
    case TokenType::StarAssign: return "'*='";
    case TokenType::SlashAssign: return "'/='";

    case TokenType::Comma: return "','";
    case TokenType::Colon: return "':'";
    case TokenType::Semicolon: return "';'";

    case TokenType::Identifier: return "identifier";
    case TokenType::Else: return "'else'";
    case TokenType::Fn: return "'fn'";
    case TokenType::Let: return "'let'";
    case TokenType::If: return "'if'";
    
    case TokenType::NumberLiteral: return "number";
    case TokenType::StringLiteral: return "string";
  }
}

std::optional<std::vector<Token>> Lexer::tokenize(std::string_view source) {
  Lexer lexer(source);
  std::vector<Token> tokens;
  while (auto token = lexer.next_token()) {
    tokens.emplace_back(std::move(*token));
  }
  tokens.emplace_back(lexer.emit(TokenType::Eof));

  if (lexer.has_error()) return std::nullopt;
  return tokens;
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
  trace::error(std::format("Lexer: ({};{}) {}", token_line, token_col, msg));
  error_flag = true;
}