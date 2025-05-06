#include "util.hpp"
#include "log.hpp"
#include <format>
#include <fstream>
#include <optional>
#include <string>
#include <vector>

std::optional<std::vector<char>> util::read_file(const std::string &filename) {
  std::ifstream stream(filename, std::ios_base::ate);
  if (!stream.is_open()) {
    trace::error(std::format("can't open file \"{}\"", filename));
    return std::nullopt;
  }

  size_t len = stream.tellg();
  stream.seekg(0);
  std::vector<char> source(len);
  stream.read(source.data(), len);

  return std::move(source);
}
