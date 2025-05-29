#pragma once

#include "lexer.hpp"
#include <initializer_list>
#include <span>
#include <stdexcept>

class TokenIterator {
public:
  TokenIterator(std::span<const Token> tokens) : tokens(tokens) {}

  bool is_eof() const { return check(TokenType::Eof); }
  const Token &peek_prev() const { return tokens[token_idx - 1]; }
  const Token &peek() const { return tokens[token_idx]; }
  const Token &peek_next() const { return tokens[token_idx + 1]; }
  bool check(TokenType type) const { return peek().type == type; }

  const Token &next() {
    const auto &token = peek();
    if (!is_eof())
      token_idx++;
    return token;
  }

  bool match(TokenType type) {
    if (check(type)) {
      next();
      return true;
    }
    return false;
  }

  bool match(std::initializer_list<TokenType> types) {
    for (const auto type : types) {
      if (match(type))
        return true;
    }
    return false;
  }

  const Token *expect(TokenType type) {
    const auto &token = peek();
    if (match(type))
      return &token;
    throw std::runtime_error("unexpected token");
  }

private:
  std::span<const Token> tokens;
  size_t token_idx = 0;
};