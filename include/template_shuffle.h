#ifndef TEMPLATE_SHUFFLE_H
#define TEMPLATE_SHUFFLE_H

// This is a template function that shuffles the elements in the range [first,
// last) using the random number generator g. It is a standard feature in C++.

#include <algorithm>
#include <limits>
// This code is meant to look like the C++ standard library.
// It is not especially readable.

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
  std::uniform_int_distribution<uint64_t> rng(
      0, std::numeric_limits<uint64_t>::max());
  __uint128_t x;
  uint64_t r = rng(g);
  uint64_t pos1, pos2;
  uint64_t val1, val2;

  for (uint64_t i = 0; i < k; i++) {
    x = (__uint128_t)(n - i) * (__uint128_t)r;
    r = (uint64_t)x;
    pos1 = n - i - 1;
    pos2 = (uint64_t)(x >> 64);
    val1 = *(storage + pos1); // should be in cache
    val2 = *(storage + pos2); // might not be in cache
    *(storage + pos1) = val2;
    *(storage + pos2) = val1; // will be read later
  }

  if (r < bound) {
    bound = n;
    for (uint64_t i = 1; i < k; i++) {
      bound *= n - i;
    }
    uint64_t t = -bound % bound;

    while (r < t) {
      r = rng(g);
      for (uint64_t i = 0; i < k; i++) {
        x = (__uint128_t)(n - i) * (__uint128_t)r;
        r = (uint64_t)x;
        pos1 = n - i - 1;
        pos2 = (uint64_t)(x >> 64);
        val1 = *(storage + pos1); // should be in cache
        val2 = *(storage + pos2); // might not be in cache
        *(storage + pos1) = val2;
        *(storage + pos2) = val1; // will be read later
      }
    }
  }

  return bound;
}


// This is a template function that shuffles the elements in the range [first,
// last)
template <class RandomIt, class URBG>
extern void shuffle_2(RandomIt first, RandomIt last, URBG &&g) {
  uint64_t i = std::distance(first, last);
  for (; i > 1 << 30; i--) {
    partial_shuffle_64b(first, i, 1, i, g);
  }

  // Batches of 2 for sizes up to 2^30 elements
  uint64_t bound = (uint64_t)1 << 60;
  for (; i > 1; i -= 2) {
    bound = partial_shuffle_64b(first, i, 2, bound, g);
  }
}


// This is a template function that shuffles the elements in the range [first,
// last)
template <class RandomIt, class URBG>
extern void shuffle_23456(RandomIt first, RandomIt last, URBG &&g) {
  uint64_t i = std::distance(first, last);
  for (; i > 1 << 30; i--) {
    partial_shuffle_64b(first, i, 1, i, g);
  }

  // Batches of 2 for sizes up to 2^30 elements
  uint64_t bound = (uint64_t)1 << 60;
  for (; i > 1 << 19; i -= 2) {
    bound = partial_shuffle_64b(first, i, 2, bound, g);
  }
  
  // Batches of 3 for sizes up to 2^19 elements
  bound = (uint64_t)1 << 57;
  for (; i > 1 << 14; i -= 3) {
    bound = partial_shuffle_64b(first, i, 3, bound, g);
  }

  // Batches of 4 for sizes up to 2^14 elements
  bound = (uint64_t)1 << 56;
  for (; i > 1 << 11; i -= 4) {
    bound = partial_shuffle_64b(first, i, 4, bound, g);
  }

  // Batches of 5 for sizes up to 2^11 elements
  bound = (uint64_t)1 << 55;
  for (; i > 1 << 9; i -= 5) {
    bound = partial_shuffle_64b(first, i, 5, bound, g);
  }

  // Batches of 6 for sizes up to 2^9 elements
  bound = (uint64_t)1 << 54;
  for (; i > 6; i -= 6) {
    bound = partial_shuffle_64b(first, i, 6, bound, g);
  }

  if (i > 1) {
    partial_shuffle_64b(first, i, i-1, 720, g);
  }
}

} // namespace batched_random

#endif // TEMPLATE_SHUFFLE_H
