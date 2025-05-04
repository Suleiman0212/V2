#include "script/vm.hpp"
#include <format>
#include <span>
#include <stdexcept>
#include "script/ast.hpp"
#include "script/cell.hpp"
#include "script/script.hpp"

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

  // clear the previous state, if any
  status = ScriptVmStatus::Running;
  frames.clear();
  frame_depth = 0;
  frames_yield_blocks.clear();

  call_inner(idx, params);
}

void ScriptVm::yield() {
  if (status != ScriptVmStatus::Running) throw std::runtime_error("vm isn't in running status");
  status = ScriptVmStatus::Yielded;

  // allocate space for saving block states
  for (size_t i = 0; i < frames.size(); i++) frames_yield_blocks.emplace_back();
}

void ScriptVm::resume() {
  if (status != ScriptVmStatus::Yielded) throw std::runtime_error("vm isn't in yielded status");
  status = ScriptVmStatus::Running;

  // restore the call stack
  auto frames_blocks = std::move(frames_yield_blocks);
  frame_depth = frames_blocks.size();
  for (size_t i = frames_blocks.size() - 1; i < frames_blocks.size(); i--) {
    auto &blocks = frames_blocks[i];
    for (size_t j = blocks.size() - 1; j < blocks.size(); j--) {
      auto &block = blocks[j];
      eval(*block.node, block.start_idx);
    }
  
    call_epilogue();
  }
}

const std::string &ScriptVm::get_string(ScriptCell idx) const {
  return script.str_table[idx];
}

ScriptCell ScriptVm::eval(const AstNodePtr &node, size_t block_start_idx) {
  auto &frame = frames.back();
  switch (node->get_type()) {
    case AstNodeType::Block: {
      auto block = static_cast<AstNodeBlock *>(node.get());
      for (size_t i = block_start_idx; i < block->nodes.size(); i++) {
        if (status == ScriptVmStatus::Yielded) {
          // save my state
          auto &blocks = frames_yield_blocks[frame_depth - 1];
					blocks.emplace(blocks.begin(), &node, i);
					break;
        }
        eval(block->nodes[i]);
      }
      break;
    }
    case AstNodeType::Literal: {
      auto literal = static_cast<AstNodeLiteral *>(node.get());
      return literal->value;
    }
    case AstNodeType::GetVar: {
      auto get_var = static_cast<AstNodeGetVar *>(node.get());
      return frame.vars[get_var->idx];
    }
    case AstNodeType::SetVar: {
      auto set_var = static_cast<AstNodeSetVar *>(node.get());
      if (set_var->idx >= frame.vars.size()) {
        // allocate more variables
        frame.vars.resize(set_var->idx + 1);
      }

      ScriptCell value = eval(set_var->value);
      frame.vars[set_var->idx] = value;
      return value;
    }
    case AstNodeType::Unary: {
      auto unary = static_cast<AstNodeUnary *>(node.get());
      switch (unary->op) {
        case UnaryOp::Group: return eval(unary->value);
        case UnaryOp::Not: return !eval(unary->value);
        case UnaryOp::Negate: return -eval(unary->value);
      }
      break;
    }
    case AstNodeType::Binary: {
      auto binary = static_cast<AstNodeBinary *>(node.get());
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
      break;
    }
    case AstNodeType::If: {
      auto if_node = static_cast<AstNodeIf *>(node.get());

      if (eval(if_node->cond)) {
        eval(if_node->true_body);
      } else if (if_node->false_body != nullptr) {
        // else
        eval(if_node->false_body);
      }
      break;
    }
    case AstNodeType::Call: {
      auto call = static_cast<AstNodeCall *>(node.get());

      std::vector<ScriptCell> params;
      for (const auto &param: call->params) params.push_back(eval(param));
      call_inner(call->idx, params);
      break;
    }
    case AstNodeType::NativeCall: {
      auto native_call = static_cast<AstNodeNativeCall *>(node.get());

      std::vector<ScriptCell> params;
      for (const auto &param: native_call->params) params.push_back(eval(param));
      const auto &native_fn = script.registry.native_fns[native_call->idx];
      return native_fn.ptr(*this, params);
    }
  }
  return 0.0;
}

void ScriptVm::call_inner(size_t idx, std::span<const ScriptCell> params) {
  frame_depth++;
  // allocate a new stack frame with the function's parameters (parameters are variables)
  frames.emplace_back(std::vector(params.begin(), params.end()));

  eval(script.fns[idx].body);
  call_epilogue();
}

void ScriptVm::call_epilogue() {
  frame_depth--;
  if (status == ScriptVmStatus::Running) {
    frames.pop_back();
    if (frames.empty()) status = ScriptVmStatus::Stopped;
  }
}