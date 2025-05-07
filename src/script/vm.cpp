#include "script/vm.hpp"
#include <array>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <vector>
#include "script/ast.hpp"
#include "script/cell.hpp"
#include "script/script.hpp"

namespace {
  AstNodePtr call_epilogue_marker = std::make_unique<AstNodeCallEpilogue>();
}

ScriptVm::ScriptVm(const Script &script): script(script) {}

ScriptVmStatus ScriptVm::get_status() const {
  return status;
}

std::optional<size_t> ScriptVm::lookup_fn(std::string_view name) const {
  return script.lookup_fn(name);
}

void ScriptVm::call(size_t idx, std::span<const ScriptCell> params) {
  const auto &fn = script.fns[idx];
  if (params.size() != fn.param_types.size()) {
    throw std::runtime_error(std::format("function expects {} parameters, but {} were passed", fn.param_types.size(), params.size()));
  }

  call_inner(idx, params);
}

void ScriptVm::yield() {
  if (status != ScriptVmStatus::Running) throw std::runtime_error("vm isn't in running status");
  status = ScriptVmStatus::Yielded;
}

void ScriptVm::resume() {
  if (status != ScriptVmStatus::Yielded) throw std::runtime_error("vm isn't in yielded status");
  exec();
}

const std::string &ScriptVm::get_string(ScriptCell idx) const {
  return script.str_table[idx];
}

void ScriptVm::call_inner(size_t idx, std::span<const ScriptCell> params) {
  frames.emplace_back(std::vector(params.begin(), params.end()));
  exec_stack.push_back(&call_epilogue_marker);
  exec_stack.push_back(&script.fns[idx].body);
  exec();
}

void ScriptVm::exec() {
  if (status == ScriptVmStatus::Running) return;

  status = ScriptVmStatus::Running;
  while (!exec_stack.empty()) {
    if (status == ScriptVmStatus::Yielded) return;

    const auto &node = *exec_stack.back();
    exec_stack.pop_back();

    switch (node->get_type()) {
      case AstNodeType::Block: {
        auto block = static_cast<const AstNodeBlock *>(node.get());
        // since this is a stack, we need to add nodes in reverse order
        for (auto it = block->nodes.rbegin(); it != block->nodes.rend(); it++) {
          exec_stack.push_back(&*it);
        }
        break;
      }
      case AstNodeType::If: {
        auto if_node = static_cast<const AstNodeIf *>(node.get());

        if (eval(if_node->cond)) {
          exec_stack.push_back(&if_node->true_body);
        } else if (if_node->false_body != nullptr) {
          // else
          exec_stack.push_back(&if_node->false_body);
        }
        break;
      }
      case AstNodeType::While: {
        auto while_node = static_cast<const AstNodeWhile *>(node.get());

        if (eval(while_node->cond)) {
          exec_stack.push_back(&node); // make sure to reevaluate the condition later
          exec_stack.push_back(&while_node->body);
        }
        break;
      }
      case AstNodeType::Call: {
        auto call = static_cast<const AstNodeCall *>(node.get());

        std::vector<ScriptCell> params;
        for (const auto &param: call->params) params.push_back(eval(param));
        call_inner(call->idx, params);
        break;
      }
      case AstNodeType::CallEpilogue: {
        // stack unwinding after a function call
        frames.pop_back();
        break;
      }
      default:
        // ...just evaluate and discard its value
        eval(node);
        break;
    }
  }

  status = ScriptVmStatus::Stopped;
}

ScriptCell ScriptVm::eval(const AstNodePtr &node) {
  auto &frame = frames.back();

  switch (node->get_type()) {
    case AstNodeType::Literal: {
      auto literal = static_cast<const AstNodeLiteral *>(node.get());
      return literal->value;
    }
    case AstNodeType::GetVar: {
      auto get_var = static_cast<const AstNodeGetVar *>(node.get());
      return frame.vars[get_var->idx];
    }
    case AstNodeType::SetVar: {
      auto set_var = static_cast<const AstNodeSetVar *>(node.get());
      if (set_var->idx >= frame.vars.size()) {
        // allocate more variables
        frame.vars.resize(set_var->idx + 1);
      }

      return frame.vars[set_var->idx] = eval(set_var->value);
    }
    case AstNodeType::DesStruct: {
      auto des_struct = static_cast<const AstNodeDesStruct *>(node.get());

      const auto &factory_fn = script.registry.native_fns[des_struct->factory_idx];
      auto factory_result = factory_fn.ptr(*this, {});

      // fields
      for (const auto &field: des_struct->fields) {
        const auto &setter_fn = script.registry.native_fns[field.setter_idx];
        setter_fn.ptr(*this, std::array{factory_result, eval(field.value)});
      }

      return factory_result;
    }
    case AstNodeType::Unary: {
      auto unary = static_cast<const AstNodeUnary *>(node.get());
      switch (unary->op) {
        case UnaryOp::Group: return eval(unary->value);
        case UnaryOp::Not: return !eval(unary->value);
        case UnaryOp::Negate: return -eval(unary->value);
      }
    }
    case AstNodeType::Binary: {
      auto binary = static_cast<const AstNodeBinary *>(node.get());
      switch (binary->op) {
        case BinaryOp::Add: return eval(binary->lhs) + eval(binary->rhs);
        case BinaryOp::Sub: return eval(binary->lhs) - eval(binary->rhs);
        case BinaryOp::Mul: return eval(binary->lhs) * eval(binary->rhs);
        case BinaryOp::Div: return eval(binary->lhs) / eval(binary->rhs);

        case BinaryOp::Eq: return eval(binary->lhs) == eval(binary->rhs);
        case BinaryOp::NotEq: return eval(binary->lhs) != eval(binary->rhs);
        case BinaryOp::Less: return eval(binary->lhs) < eval(binary->rhs);
        case BinaryOp::LessEq: return eval(binary->lhs) <= eval(binary->rhs);
        case BinaryOp::Greater: return eval(binary->lhs) > eval(binary->rhs);
        case BinaryOp::GreaterEq: return eval(binary->lhs) >= eval(binary->rhs);
      }
    }
    case AstNodeType::NativeCall: {
      auto native_call = static_cast<const AstNodeNativeCall *>(node.get());
      const auto &native_fn = script.registry.native_fns[native_call->idx];

      std::vector<ScriptCell> params;
      for (const auto &param: native_call->params) params.push_back(eval(param));
      return native_fn.ptr(*this, params);
    }
    default: return 0.0;
  }
}
