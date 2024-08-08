
/**
 * This header contains C++ helper templates. They are not meant for end users.
 */
#ifndef PARTIAL_SHUFFLE_INL_H
#define PARTIAL_SHUFFLE_INL_H

#include <algorithm>
#include <cstdint>

#include "partial-shuffle-inl.h"

namespace batched_random {

// Performs k steps of a Fisher-Yates shuffle on n elements, in the array
// `storage`.
//
// Preconditions:
//   n >= k >= 1
//   bound >= n*(n-1)*...*(n-(k-1)), which must not overflow
//   rng() produces uniformly random 64-bit values
//
// The return value is usable as `bound` for smaller batches of size k.
template <class RandomIt, class URBG>
inline uint64_t partial_shuffle_64b(RandomIt storage, uint64_t n, uint64_t k,
                                    uint64_t bound, URBG &g) {
  static_assert(std::is_same<typename URBG::result_type, uint64_t>::value, "result_type must be uint64_t");
  __uint128_t x;
  uint64_t r = g();
  uint64_t indexes[7]; // We know that k <= 7

  for (uint64_t i = 0; i < k; i++) {
    x = (__uint128_t)(n - i) * (__uint128_t)r;
    r = (uint64_t)x;
    indexes[i] = (uint64_t)(x >> 64);
  }

  if (r < bound) {
    bound = n;
    for (uint64_t i = 1; i < k; i++) {
      bound *= n - i;
    }
    uint64_t t = -bound % bound;

    while (r < t) {
      r = g();
      for (uint64_t i = 0; i < k; i++) {
        x = (__uint128_t)(n - i) * (__uint128_t)r;
        r = (uint64_t)x;
        indexes[i] = (uint64_t)(x >> 64);
      }
    }
  }
  for (uint64_t i = 0; i < k; i++) {
    std::iter_swap(storage + n - i - 1, storage + indexes[i]);
  }

  return bound;
}

} // namespace batched_random

#endif // TEMPLATE_SHUFFLE_H