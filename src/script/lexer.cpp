#include "script/lexer.hpp"
#include <charconv>
#include <format>
#include <string>
#include "log.hpp"

std::optional<std::vector<Token>> Lexer::tokenize(std::string_view source) {
    Lexer lexer(source);
    std::vector<Token> tokens;
    while (auto token = lexer.next_token()) {
        tokens.emplace_back(std::move(*token));
    }
    return tokens;
}

Lexer::Lexer(std::string_view source): source(source) {}

std::optional<Token> Lexer::next_token() {
again:
    token_pos = pos;
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
        case '"': return read_string();
        default: {
            if (isspace(ch)) {
                goto again;
                break;
            } else if (isdigit(ch)) {
                return read_number();
            } else if (isalpha(ch) || ch == '_') {
                return read_identifier();
            }

            trace::error(std::format("unknown token '{}'", ch));
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
        trace::error("malformed number");
        return std::nullopt;
    }
	return emit(TokenType::NumberLiteral, value);
}

Token Lexer::emit(TokenType type) {
    return Token(type);
}

Token Lexer::emit(TokenType type, double value) {
    return Token(type, value);
}

Token Lexer::emit(TokenType type, std::string_view value) {
    return Token(type, std::string(value));
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
    return source[pos++];
}

bool Lexer::match(char ch) {
    if (peek() != ch) return false;
	next();
	return true;
}