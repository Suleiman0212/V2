#pragma once

#include <string_view>

enum class LogLevel {
  Max,
  Min,
  Info,
  Warn,
  Debug,
  Error,
};

namespace trace {
  extern LogLevel level;

  void info(std::string_view msg);
  void warn(std::string_view msg);
  void debug(std::string_view msg);
  void error(std::string_view msg);
}
