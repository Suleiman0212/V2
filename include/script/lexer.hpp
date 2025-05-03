#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

enum class TokenType {
	LeftParen,
	RightParen,
	LeftBrace,
	RightBrace,
	
	Not,
	Plus,
	Minus,
	Star,
	Slash,

	Eq,
	NotEq,
	Less,
	LessEq,
	Greater,
	GreaterEq,
	
	Assign,
	PlusAssign,
	MinusAssign,
	StarAssign,
	SlashAssign,

	Comma,
	Colon,
	Semicolon,

	Identifier, // string value
	Else,
	Fn,
	Let,
	If,

	NumberLiteral, // double value
	StringLiteral, // string value
};

struct Token {
	double as_double() const { return std::get<double>(value); }
	const std::string &as_string() const { return std::get<std::string>(value); }

	TokenType type;
	std::variant<std::monostate, double, std::string> value;
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

	Token emit(TokenType type);
	Token emit(TokenType type, double value);
	Token emit(TokenType type, std::string_view value);

	bool is_eof();
	char peek();
	char peek_prev();
	char peek_next();

	char next();
	bool match(char ch);

	std::string_view source;
	size_t pos = 0;
	size_t token_pos = 0;
};