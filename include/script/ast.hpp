#pragma once

#include <memory>
#include <vector>
#include "cell.hpp"

enum class AstNodeType {
  Block,

  Literal,
  GetVar,
  SetVar,
  DesStruct,

  Unary,
  Binary,

  If,
  While,

  Call,
  CallEpilogue, // vm internal use
  NativeCall,
};

enum class UnaryOp {
  Group,
  Not,
  Negate,
};

enum class BinaryOp {
  Add,
  Sub,
  Mul,
  Div,

  Eq,
  NotEq,
  Less,
  LessEq,
  Greater,
  GreaterEq,
};

struct AstNode {
  AstNode() = default;
  AstNode(ScriptCellType value_type): value_type(value_type) {}
  virtual ~AstNode() = default;

  virtual AstNodeType get_type() const = 0;

  ScriptCellType value_type = ScriptCellType::Void;
};
using AstNodePtr = std::unique_ptr<AstNode>;

struct AstNodeBlock: AstNode {
  AstNodeBlock(std::vector<AstNodePtr> &&nodes): nodes(std::move(nodes)) {}

  AstNodeType get_type() const override { return AstNodeType::Block; }

  std::vector<AstNodePtr> nodes;
};

struct AstNodeLiteral: AstNode {
  AstNodeLiteral(ScriptCellType type, ScriptCell value): AstNode(type), value(value) {}

  AstNodeType get_type() const override { return AstNodeType::Literal; }

  ScriptCell value;
};

struct AstNodeGetVar: AstNode {
  AstNodeGetVar(size_t idx, ScriptCellType type): AstNode(type), idx(idx) {}

  AstNodeType get_type() const override { return AstNodeType::GetVar; }

  size_t idx;
};

struct AstNodeSetVar: AstNode {
  AstNodeSetVar(size_t idx, AstNodePtr &&value): AstNode(value->value_type), idx(idx), 
    value(std::move(value)) {}

  AstNodeType get_type() const override { return AstNodeType::SetVar; }

  size_t idx;
  AstNodePtr value;
};

struct AstNodeDesStruct: AstNode {
  struct Field {
    size_t setter_idx;
    AstNodePtr value;
  };

  AstNodeDesStruct(size_t factory_idx, std::vector<Field> &&fields): AstNode(ScriptCellType::Number),
    factory_idx(factory_idx), fields(std::move(fields)) {}

  AstNodeType get_type() const override { return AstNodeType::DesStruct; }

  size_t factory_idx;
  std::vector<Field> fields;
};

struct AstNodeUnary: AstNode {
  AstNodeUnary(UnaryOp op, AstNodePtr &&value): AstNode(value->value_type), op(op), 
    value(std::move(value)) {}

  AstNodeType get_type() const override { return AstNodeType::Unary; }

  UnaryOp op;
  AstNodePtr value;
};

struct AstNodeBinary: AstNode {
  AstNodeBinary(BinaryOp op, AstNodePtr &&lhs, AstNodePtr &&rhs): AstNode(lhs->value_type), op(op), 
    lhs(std::move(lhs)), rhs(std::move(rhs)) {}

  AstNodeType get_type() const override { return AstNodeType::Binary; }

  BinaryOp op;
  AstNodePtr lhs, rhs;
};

struct AstNodeIf: AstNode {
  AstNodeIf(AstNodePtr &&cond, AstNodePtr &&true_body, AstNodePtr &&false_body): cond(std::move(cond)), 
    true_body(std::move(true_body)), false_body(std::move(false_body)) {}

  AstNodeType get_type() const override { return AstNodeType::If; }

  AstNodePtr cond;
  AstNodePtr true_body, false_body;
};

struct AstNodeWhile: AstNode {
  AstNodeWhile(AstNodePtr &&cond, AstNodePtr &&body): cond(std::move(cond)), body(std::move(body)) {}

  AstNodeType get_type() const override { return AstNodeType::While; }

  AstNodePtr cond;
  AstNodePtr body;
};

struct AstNodeCall: AstNode {
  AstNodeCall(size_t idx, std::vector<AstNodePtr> &&params): idx(idx), params(std::move(params)) {}

  AstNodeType get_type() const override { return AstNodeType::Call; }

  size_t idx;
  std::vector<AstNodePtr> params;
};

struct AstNodeCallEpilogue: AstNode {
  AstNodeType get_type() const override { return AstNodeType::CallEpilogue; }
};

struct AstNodeNativeCall: AstNode {
  AstNodeNativeCall(size_t idx, ScriptCellType return_type, std::vector<AstNodePtr> &&params): 
    AstNode(return_type), idx(idx), params(std::move(params)) {}

  AstNodeType get_type() const override { return AstNodeType::NativeCall; }

  size_t idx;
  std::vector<AstNodePtr> params;
};