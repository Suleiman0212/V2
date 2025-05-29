#pragma once

#include "ast.hpp"
#include "cell.hpp"
#include "registry.hpp"
#include "script.hpp"
#include <optional>
#include <string_view>
#include <vector>

enum class ScriptVmStatus {
  Stopped,
  Running,
  Yielded,
};

class ScriptVm {
public:
  ScriptVm(const Script &script);

  ScriptVmStatus get_status() const;

  std::optional<size_t> lookup_fn(std::string_view name) const;
  void call(size_t idx, ScriptParams params = {});
  void yield();
  void resume();

  const std::string &get_string(ScriptCell idx) const;

private:
  struct CallFrame {
    std::vector<ScriptCell> vars;
  };

  struct YieldBlockState {
    const AstNodePtr *node;
    size_t start_idx;
  };

  void call_inner(size_t idx, ScriptParams params);
  void exec();

  ScriptCell eval(const AstNodePtr &node);

  const Script &script;

  ScriptVmStatus status = ScriptVmStatus::Stopped;
  std::vector<CallFrame> frames;
  std::vector<const AstNodePtr *> exec_stack;

  friend class Parser;
};