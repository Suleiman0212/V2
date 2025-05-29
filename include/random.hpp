#pragma once

#include <cstdint>

namespace rng {
void seed(uint64_t seed);

uint64_t next_uint();
float next_float();

int next_int_range(int min, int max);
float next_float_range(float min, float max);
} // namespace rng