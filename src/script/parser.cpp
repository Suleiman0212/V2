#include "script/parser.hpp"
#include "log.hpp"
#include "script/ast.hpp"
#include "script/cell.hpp"
#include "script/lexer.hpp"
#include "util.hpp"
#include <algorithm>
#include <format>
#include <span>
#include <string_view>

std::optional<Script> Parser::parse(std::span<const Token> tokens, const ScriptRegistry &registry) {
  Script script(registry);
  Parser parser(tokens, script);
  while (!parser.is_eof()) parser.top_level();

  if (parser.has_error()) return std::nullopt;
  return script;
}

Parser::Parser(std::span<const Token> tokens, Script &script): tokens(tokens), script(script) {}

bool Parser::is_eof() {
  return check(TokenType::Eof);
}

const Token &Parser::peek() {
  return tokens[token_idx];
}

const Token &Parser::peek_prev() {
  return tokens[token_idx - 1];
}

const Token &Parser::peek_next() {
  if (is_eof()) return peek();
  return tokens[token_idx + 1];
}

bool Parser::check(TokenType type) {
  return peek().type == type;
}

const Token &Parser::next() {
  if (!is_eof()) token_idx++;
  return peek_prev();
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    next();
    return true;
  }
  return false;
}

bool Parser::match(std::initializer_list<TokenType> types) {
  for (auto type: types) {
    if (check(type)) {
      next();
      return true;
    }
  }
  return false;
}

const Token *Parser::expect(TokenType type) {
  const auto &token = peek();
  if (match(type)) return &token;
  throw_error(std::format("expected {}, got {}", Token::type_name(type), token.type_name()), token.line, token.col);
  next();
  return nullptr;
}

Parser::Symbol Parser::lookup_symbol(std::string_view name, size_t &idx) {
  if (auto native_fn_idx = script.registry.lookup_native_fn(name)) {
    idx = *native_fn_idx;
    return Symbol::NativeFn;
  } else if (auto fn_idx = script.lookup_fn(name)) {
    idx = *fn_idx;
    return Symbol::Fn;
  } else {
    // variable?
    auto var_idx = util::index_of(vars.begin(), vars.end(), [name](const auto &x) {
      return x.name == name;
    });
    if (var_idx) {
      idx = *var_idx;
      return Symbol::Var;
    }

    return Symbol::None;
  }
}

std::string_view Parser::symbol_name(Symbol sym) {
  switch (sym) {
    case Symbol::None: return "none";
    case Symbol::Var: return "variable";
    case Symbol::Fn: return "function";
    case Symbol::NativeFn: return "native function";
  }
  return "<?>";
}

std::string_view Parser::cell_type_name(ScriptCellType type) {
  switch (type) {
    case ScriptCellType::Void: return "void";
    case ScriptCellType::Number: return "number";
    case ScriptCellType::String: return "string";
    case ScriptCellType::FnHandle: return "function handle";
  }
  return "<?>";
}

std::string_view Parser::unary_op_name(UnaryOp op) {
  switch (op) {
    case UnaryOp::Group: return "'()'";
    case UnaryOp::Not: return "'!'";
    case UnaryOp::Negate: return "'-'";
  }
  return "<?>";
}

std::string_view Parser::binary_op_name(BinaryOp op) {
  switch (op) {
    case BinaryOp::Add: return "+";
    case BinaryOp::Sub: return "-";
    case BinaryOp::Mul: return "*";
    case BinaryOp::Div: return "/";

    case BinaryOp::Eq: return "==";
    case BinaryOp::NotEq: return "!=";
    case BinaryOp::Less: return "<";
    case BinaryOp::LessEq: return "<=";
    case BinaryOp::Greater: return ">";
    case BinaryOp::GreaterEq: return ">=";
  }
  return "<?>";
}

AstNodePtr Parser::unary(UnaryOp op, AstNodePtr &&value, int line, int col) {
  if (value->value_type != ScriptCellType::Number) {
    throw_error(std::format("can't perform unary {} on {}", unary_op_name(op), cell_type_name(value->value_type)),
      line, col);
  }

  return new_node<AstNodeUnary>(op, std::move(value));
}

AstNodePtr Parser::fn_ref(bool native, size_t idx) {
  if (!match(TokenType::LeftParen)) {
    // function handle
    const auto &token = peek();
    if (native) {
      throw_error("native functions cannot be referenced", token.line, token.col);
      return new_node<AstNodeLiteral>(ScriptCellType::Number, 0.0);
    }
    return new_node<AstNodeLiteral>(ScriptCellType::FnHandle, idx);
  }
  const auto &paren_token = peek_prev();

  // parameters
  std::vector<AstNodePtr> params;
  std::vector<const Token *> param_tokens;
  while (!is_eof() && !check(TokenType::RightParen)) {
    if (!params.empty()) expect(TokenType::Comma);

    const auto &token = peek();
    params.emplace_back(expr());
    param_tokens.push_back(&token);
  }
  expect(TokenType::RightParen);

  ScriptCellType fn_return_type = ScriptCellType::Void;
  std::span<const ScriptCellType> fn_param_types;
  if (native) {
    const auto &native_fn = script.registry.native_fns[idx];
    fn_return_type = native_fn.return_type;
    fn_param_types = native_fn.param_types;
  } else {
    // script functions can't return values
    fn_param_types = script.fns[idx].param_types;
  }

  // verify parameters
  if (params.size() != fn_param_types.size()) {
    throw_error(std::format("function expects {} parameters, but {} were passed", fn_param_types.size(), params.size()),
      paren_token.line, paren_token.col);
  }
  for (size_t i = 0; i < std::min(params.size(), fn_param_types.size()); i++) {
    auto got = params[i]->value_type,
         expected = fn_param_types[i];
    const auto &token = *param_tokens[i];
    if (got != expected) {
      throw_error(std::format("parameter #{} is expected to be a {}, but {} was passed", i + 1, cell_type_name(expected), cell_type_name(got)),
        token.line, token.col);
    }
  }

  // ok, call it!
  if (native) {
    return new_node<AstNodeNativeCall>(idx, fn_return_type, std::move(params));
  } else {
    return new_node<AstNodeCall>(idx, std::move(params));
  }
}

AstNodePtr Parser::primary() {
  if (match(TokenType::NumberLiteral)) {
    auto value = peek_prev().as_double();
    return new_node<AstNodeLiteral>(ScriptCellType::Number, value);
  } else if (match(TokenType::StringLiteral)) {
    const auto &value = peek_prev().as_string();
    
    size_t str_table_idx = script.add_string(value);
    return new_node<AstNodeLiteral>(ScriptCellType::String, str_table_idx);
  } else if (match(TokenType::Identifier)) {
    const auto &token = peek_prev();
    const auto &identifier = token.as_string();

    // what is it exactly?
    size_t sym_idx;
    Symbol sym = lookup_symbol(identifier, sym_idx);
    switch (sym) {
      case Symbol::None:
        throw_error(std::format("unknown identifier \"{}\"", identifier), token.line, token.col);
        if (match(TokenType::LeftParen)) {
          // non existent function, just eat this "call" up
          bool first = true;
          while (!is_eof() && !check(TokenType::RightParen)) {
            if (!first) expect(TokenType::Comma);

            next();
            first = false;
          }
          expect(TokenType::RightParen);
        }
        break;
      case Symbol::Var: return new_node<AstNodeGetVar>(sym_idx, vars[sym_idx].type);
      case Symbol::Fn: return fn_ref(false, sym_idx);
      case Symbol::NativeFn: return fn_ref(true, sym_idx);
    }
  } else if (match(TokenType::LeftParen)) {
    // grouping
    AstNodePtr value = expr();
    expect(TokenType::RightParen);
    return unary(UnaryOp::Group, std::move(value));
  } else if (match(TokenType::Not)) {
    return unary(UnaryOp::Not, primary());
  } else if (match(TokenType::Minus)) {
    return unary(UnaryOp::Negate, primary());
  } else {
    const auto &token = next();
    throw_error(std::format("expected number, string, identifier, '(', '!' or '-'; got {}", token.type_name()), token.line, token.col);
  }

  std::vector<AstNodePtr> dummy;
  return new_node<AstNodeBlock>(std::move(dummy));
}

AstNodePtr Parser::binary(BinaryOp op, AstNodePtr &&lhs, AstNodePtr &&rhs, int line, int col) {
  auto lhs_type = lhs->value_type,
       rhs_type = rhs->value_type;
  bool allow = true;
  if (lhs_type == ScriptCellType::Void || rhs_type == ScriptCellType::Void || lhs_type != rhs_type) {
    allow = false;
  } else if (op != BinaryOp::Eq && op != BinaryOp::NotEq) {
    // arithmetic operator, only allow numbers
    allow = lhs_type == ScriptCellType::Number && rhs_type == ScriptCellType::Number;
  }
  if (!allow) {
    throw_error(std::format("can't perform binary '{}' on {} and {}", binary_op_name(op), cell_type_name(lhs_type), cell_type_name(rhs_type)), 
      line, col);
  }

  return new_node<AstNodeBinary>(op, std::move(lhs), std::move(rhs));
}

AstNodePtr Parser::factor() {
  AstNodePtr lhs = primary();

  while (match({TokenType::Star, TokenType::Slash})) {
    const auto &op_token = peek_prev();
    BinaryOp op = (op_token.type == TokenType::Star)? BinaryOp::Mul : BinaryOp::Div;
    lhs = binary(op, std::move(lhs), primary(), op_token.line, op_token.col);
  }

  return lhs;
}

AstNodePtr Parser::term() {
  AstNodePtr lhs = factor();

  while (match({TokenType::Plus, TokenType::Minus})) {
    const auto &op_token = peek_prev();
    BinaryOp op = (op_token.type == TokenType::Plus)? BinaryOp::Add : BinaryOp::Sub;
    lhs = binary(op, std::move(lhs), factor(), op_token.line, op_token.col);
  }

  return lhs;
}

AstNodePtr Parser::test() {
  AstNodePtr lhs = term();

  static constexpr auto TEST_TOKENS = {
    TokenType::Eq, TokenType::NotEq,
    TokenType::Less, TokenType::LessEq,
    TokenType::Greater, TokenType::GreaterEq,
  };
  while (match(TEST_TOKENS)) {
    const auto &op_token = peek_prev();
    BinaryOp op = BinaryOp::Eq;
    switch (op_token.type) {
      case TokenType::NotEq:
        op = BinaryOp::NotEq;
        break;
      case TokenType::Less:
        op = BinaryOp::Less;
        break;
      case TokenType::LessEq:
        op = BinaryOp::LessEq;
        break;
      case TokenType::Greater:
        op = BinaryOp::Greater;
        break;
      case TokenType::GreaterEq:
        op = BinaryOp::GreaterEq;
        break;
      default: break;
    }

    lhs = binary(op, std::move(lhs), term(), op_token.line, op_token.col);
  }

  return lhs;
}

AstNodePtr Parser::expr() {
  if (!check(TokenType::Identifier)) return test();
  AstNodePtr lhs = test();

  static constexpr auto ASSIGN_TOKENS = {
    TokenType::Assign, 
    TokenType::PlusAssign, TokenType::MinusAssign, 
    TokenType::StarAssign, TokenType::SlashAssign,
  };
  if (lhs->get_type() == AstNodeType::GetVar && match(ASSIGN_TOKENS)) {
    size_t var_idx = static_cast<AstNodeGetVar *>(lhs.get())->idx;

    const auto &op_token = peek_prev();
    const auto &rhs_token = peek();
    AstNodePtr rhs = expr();
    if (rhs->value_type == lhs->value_type) {
      // shorthand assignment operator?
      switch (op_token.type) {
        case TokenType::PlusAssign:
          rhs = binary(BinaryOp::Add, std::move(lhs), std::move(rhs), op_token.line, op_token.col);
          break;
        case TokenType::MinusAssign:
          rhs = binary(BinaryOp::Sub, std::move(lhs), std::move(rhs), op_token.line, op_token.col);
          break;
        case TokenType::StarAssign:
          rhs = binary(BinaryOp::Mul, std::move(lhs), std::move(rhs), op_token.line, op_token.col);
          break;
        case TokenType::SlashAssign:
          rhs = binary(BinaryOp::Div, std::move(lhs), std::move(rhs), op_token.line, op_token.col);
          break;
        default: break;
      }
    } else {
      throw_error(std::format("can't assign {} to a {} variable", cell_type_name(rhs->value_type), cell_type_name(lhs->value_type)), 
        rhs_token.line, rhs_token.col);
    }

    lhs = new_node<AstNodeSetVar>(var_idx, std::move(rhs));
  }

  return lhs;
}

AstNodePtr Parser::if_statement() {
  const auto &cond_token = peek();
  AstNodePtr cond = expr();
  if (cond->value_type != ScriptCellType::Number) {
    throw_error(std::format("can't use {} expression in if condition", cell_type_name(cond->value_type)), 
      cond_token.line, cond_token.col);
  }

  expect(TokenType::LeftBrace);
  AstNodePtr true_body = block(),
             false_body = nullptr;
  if (match(TokenType::Else)) {
    if (match(TokenType::If)) {
      // else if
      false_body = if_statement();
    } else {
      expect(TokenType::LeftBrace);
      false_body = block();
    }
  }

  return new_node<AstNodeIf>(std::move(cond), std::move(true_body), std::move(false_body));
}

AstNodePtr Parser::var_decl() {
  std::string_view name = "<?>";
  if (auto token = expect(TokenType::Identifier)) {
    name = token->as_string();

    // already defined?
    size_t sym_idx;
    Symbol sym = lookup_symbol(name, sym_idx);
    if (sym != Symbol::None) {
      throw_error(std::format("redefinition of symbol \"{}\" (a {})", name, symbol_name(sym)), token->line, token->col);
    }
  }

  expect(TokenType::Assign);
  const auto &value_token = peek();
  AstNodePtr value = expr();
  if (value->value_type == ScriptCellType::Void) {
    throw_error("can't initialize variable with void expression", value_token.line, value_token.col);
  }
  expect(TokenType::Semicolon);

  // acknowledge its existence
  size_t idx = vars.size();
  vars.emplace_back(std::string(name), value->value_type, scope_depth);
  return new_node<AstNodeSetVar>(idx, std::move(value));
}

AstNodePtr Parser::block() {
  scope_depth++;

  std::vector<AstNodePtr> nodes;
  while (!is_eof() && !check(TokenType::RightBrace)) {
    nodes.emplace_back(statement());
  }
  expect(TokenType::RightBrace);

  // forget about variables from this scope
  while (!vars.empty()) {
    if (vars.back().depth != scope_depth) break;
    vars.pop_back();
  }
  scope_depth--;

  return new_node<AstNodeBlock>(std::move(nodes));
}

AstNodePtr Parser::statement() {
  if (match(TokenType::LeftBrace)) {
    return block();
  } else if (match(TokenType::Let)) {
    return var_decl();
  } else if (match(TokenType::If)) {
    return if_statement();
  } else {
    // expression
    AstNodePtr node = expr();
    expect(TokenType::Semicolon);
    return node;
  }
}

void Parser::fn_decl() {
  std::string_view name = "";
  if (auto token = expect(TokenType::Identifier)) name = token->as_string();
  expect(TokenType::LeftParen);

  // parameters
  std::vector<ScriptCellType> param_types;
  while (!is_eof() && !check(TokenType::RightParen)) {
    if (!param_types.empty()) expect(TokenType::Comma);

    std::string_view param_name = "";
    if (auto token = expect(TokenType::Identifier)) param_name = token->as_string();
    ScriptCellType type = ScriptCellType::Void;
    
    // type
    expect(TokenType::Colon);
    if (auto token = expect(TokenType::Identifier)) {
      const auto &type_name = token->as_string();
      if (type_name == "number") {
        type = ScriptCellType::Number;
      } else if (type_name == "string") {
        type = ScriptCellType::String;
      } else {
        throw_error(std::format("unknown type name \"{}\"", type_name), token->line, token->col);
      }
    }

    // pretend that this is a variable
    param_types.push_back(type);
    vars.emplace_back(std::string(param_name), type, 1);
  }
  expect(TokenType::RightParen);

  // parse its body & acknowledge its existence
  expect(TokenType::LeftBrace);
  script.fns.emplace_back(std::string(name), std::move(param_types), block());
}

void Parser::top_level() {
  if (match(TokenType::Fn)) {
    fn_decl();
  } else {
    const auto &token = peek();
    throw_error(std::format("expected 'fn', got {}", token.type_name()), token.line, token.col);
    next();
  }
}

bool Parser::has_error() {
  return error_flag;
}

void Parser::throw_error(std::string_view msg, int line, int col) {
  if (line != 0 && col != 0) {
    trace::error(std::format("Parser: ({};{}) {}", line, col, msg));
  } else {
    trace::error(std::format("Parser: {}", msg));
  }
  error_flag = true;
}