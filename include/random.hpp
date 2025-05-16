#pragma once

#include <climits>
#include <cstdint>

/// Set the seed for randomizer
void rand_seed(uint64_t seed);
/// Return random value in range 0 - UINT64_MAX, seed is initialized
/// automatically
uint64_t rand_uint();
/// Return random value in range 0.0 - 1.0, seed is initialized automatically
float rand_float();
/// Return random value in range min - max, seed is initialized automatically
int rand_range(int min, int max);
/// Return random value in range min - max, seed is initialized automatically
float rand_range(float min, float max);
