#include "random.hpp"
#include <array>
#include <chrono>

namespace {
std::array<uint64_t, 4> xoshiro_state{0, 0, 0, 0};

uint64_t splitmix64(uint64_t &state) {
  state += 0x9e3779b97f4a7c15;
  uint64_t t = state;
  t = (t ^ (t >> 30)) * 0xbf58476d1ce4e5b9;
  t = (t ^ (t >> 27)) * 0x94d049bb133111eb;
  return t ^ (t >> 31);
}

static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
} // namespace

void rng::seed(uint64_t seed) {
  uint64_t state = seed;
  xoshiro_state[0] = splitmix64(state);
  xoshiro_state[1] = splitmix64(state);
  xoshiro_state[2] = splitmix64(state);
  xoshiro_state[3] = splitmix64(state);
}

uint64_t rng::next_uint() {
  using namespace std::chrono;

  if (xoshiro_state[0] == 0 && xoshiro_state[1] == 0 && xoshiro_state[2] == 0 &&
      xoshiro_state[3] == 0) {
    uint64_t time_ms = duration_cast<milliseconds>(
                           high_resolution_clock::now().time_since_epoch())
                           .count();
    seed(time_ms);
  }

  uint64_t result =
      rotl(xoshiro_state[0] + xoshiro_state[3], 23) + xoshiro_state[0];

  uint64_t t = xoshiro_state[1] << 17;
  xoshiro_state[2] ^= xoshiro_state[0];
  xoshiro_state[3] ^= xoshiro_state[1];
  xoshiro_state[1] ^= xoshiro_state[2];
  xoshiro_state[0] ^= xoshiro_state[3];
  xoshiro_state[2] ^= t;
  xoshiro_state[3] = rotl(xoshiro_state[3], 45);

  return result;
}

float rng::next_float() { return next_uint() / (float)UINT64_MAX; }

int rng::next_int_range(int min, int max) { return next_float_range(min, max); }

float rng::next_float_range(float min, float max) {
  return (max + std::abs(min)) * next_float() + min;
}
