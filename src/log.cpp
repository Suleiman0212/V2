#include "log.hpp"
#include <iostream>

LogLevel trace::level = LogLevel::Max;

namespace {  
  void log_out(std::string_view msg, LogLevel level) {
    if (level != LogLevel::Error && trace::level != LogLevel::Max && level != trace::level) {
      return;
    }

    const char *prefix;
    std::ostream *stream = &std::cout;
    switch (level) {
      case LogLevel::Info: {
        prefix = "\033[34mINFO\033[0m: ";
        break;
      }
      case LogLevel::Warn: {
        prefix = "\033[33mWARN\033[0m: ";
        break;
      }
      case LogLevel::Debug: {
        prefix = "\033[32mDEBUG\033[0m: ";
        break;
      }
      case LogLevel::Error: {
        prefix = "\033[31mERROR\033[0m: ";
        stream = &std::cerr;
        break;
      }
      default: break;
    }

    *stream << prefix << msg << std::endl;
  }
}

void trace::info(std::string_view msg) {
  log_out(msg, LogLevel::Info);
}

void trace::warn(std::string_view msg) {
  log_out(msg, LogLevel::Warn);
}

void trace::debug(std::string_view msg) {
  log_out(msg, LogLevel::Debug);
}

void trace::error(std::string_view msg) {
  log_out(msg, LogLevel::Error);
}
