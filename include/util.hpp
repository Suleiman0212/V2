#pragma once

#include <algorithm>
#include <cstddef>
#include <optional>

namespace util {
  template<class InputIt, class Pred>
  std::optional<size_t> index_of(InputIt first, InputIt last, Pred pred) {
    auto it = std::find_if(first, last, pred);
    if (it == last) return std::nullopt;
    return std::distance(first, it);
  }
}