#ifndef BENCHMARKS_GENERATORS_H
#define BENCHMARKS_GENERATORS_H
#include <random>

class lehmer64 {
public:
  using result_type = uint64_t;
  static constexpr result_type(min)() { return 0; }
  static constexpr result_type(max)() { return UINT64_MAX; }

  lehmer64() : m_state(1234) {}
  void step() { m_state *= UINT64_C(0xda942042e4dd58b5); }
  lehmer64(std::random_device &rd) { seed(rd); }
  lehmer64(std::random_device &&rd) { seed(rd); }

  void seed(std::random_device &rd) {
    uint64_t s0 = uint64_t(rd()) << 31 | uint64_t(rd());
    uint64_t s1 = uint64_t(rd()) << 31 | uint64_t(rd());
    m_state = (__uint128_t)s0 << 64 | s1;
  }

  result_type operator()() {
    step();
    return (uint64_t)(m_state >> 64);
  }

  void discard(unsigned long long n) {
    for (unsigned long long i = 0; i < n; ++i)
      operator()();
  }

private:
  __uint128_t m_state;
};

#endif