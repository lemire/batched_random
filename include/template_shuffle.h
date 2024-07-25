
/**
 * This header contains C++ templates that shuffle the elements in the range
 * [first, last) using the random number generator g. They are meant to emulate
 * the standard std::shuffle function and can often act as drop-in replacement.
 */
#ifndef TEMPLATE_SHUFFLE_H
#define TEMPLATE_SHUFFLE_H

#include "partial-shuffle-inl.h"

// This code is meant to look like the C++ standard library.
namespace batched_random {

namespace internal {
template <class RandomIt, class URBG>
void just1(RandomIt first, uint64_t i, URBG &g) {
  for (; i > (1 << 30); i--) {
    uint64_t index = partial_shuffle_64_1(i, i, g);
    std::iter_swap(first + i - 1, first + index);
  }
}
// Batches of 2 for sizes up to 2^30 elements
template <class RandomIt, class URBG>
uint64_t just2(RandomIt first, uint64_t i, URBG &g) {
  uint64_t bound = (uint64_t)1 << 60;
  uint64_t indexes[2];
  for (; i > 1 << 19; i -= 2) {
    bound = partial_shuffle_64_k<2>(i, bound, g, indexes);
    for(uint64_t j = 0; j < 2; j++) {
      std::iter_swap(first + i - j - 1, first + indexes[j]);
    }
  }
  return i;
}
// Batches of 2 for sizes up to 2^30 elements
template <class RandomIt, class URBG>
uint64_t just2final(RandomIt first, uint64_t i, URBG &g) {
  uint64_t bound = (uint64_t)1 << 60;
  uint64_t indexes[2];
  for (; i > 1; i -= 2) {
    bound = partial_shuffle_64_k<2>(i, bound, g, indexes);
    for(uint64_t j = 0; j < 2; j++) {
      std::iter_swap(first + i - j - 1, first + indexes[j]);
    }
  }
  return i;
}
// Batches of 3 for sizes up to 2^19 elements
template <class RandomIt, class URBG>
uint64_t just3(RandomIt first, uint64_t i, URBG &g) {
  uint64_t bound = (uint64_t)1 << 57;
  uint64_t indexes[3];
  for (; i > (1 << 14); i -= 3) {
    bound = partial_shuffle_64_k<3>(i, bound, g, indexes);
    for(uint64_t j = 0; j < 3; j++) {
      std::iter_swap(first + i - j - 1, first + indexes[j]);
    }
  }
  return i;
}

// Batches of 4 for sizes up to 2^14 elements
template <class RandomIt, class URBG>
uint64_t just4(RandomIt first, uint64_t i, URBG &g) {
  uint64_t bound = (uint64_t)1 << 56;
  uint64_t indexes[4];
  for (; i > (1 << 11); i -= 4) {
    bound = partial_shuffle_64_k<4>(i, bound, g, indexes);
    for(uint64_t j = 0; j < 4; j++) {
      std::iter_swap(first + i - j - 1, first + indexes[j]);
    }
  }
  return i;
}

// Batches of 5 for sizes up to 2^11 elements
template <class RandomIt, class URBG>
uint64_t just5(RandomIt first, uint64_t i, URBG &g) {
  uint64_t bound = (uint64_t)1 << 55;
  uint64_t indexes[5];
  for (; i > (1 << 9); i -= 5) {
    bound = partial_shuffle_64_k<5>( i, bound, g, indexes);
    for(uint64_t j = 0; j < 5; j++) {
      std::iter_swap(first + i - j - 1, first + indexes[j]);
    }
  }
  return i;
}

template <class RandomIt, class URBG>
inline uint64_t just6(RandomIt first, uint64_t i, URBG &g) {
  uint64_t bound = (uint64_t)1 << 54;
  uint64_t indexes[6];
  for (; i > 6; i -= 6) {
    bound = partial_shuffle_64_k<6>(i, bound, g, indexes);
    for(uint64_t j = 0; j < 6; j++) {
      std::iter_swap(first + i - j - 1, first + indexes[j]);
    }
  }
  return i;
}
} // namespace internal

// This is a template function that shuffles the elements in the range [first,
// last).
//
// It is similar to std::shuffle, but it uses a different algorithm.
template <class RandomIt, class URBG>
extern void shuffle_2(RandomIt first, RandomIt last, URBG &&g) {
  uint64_t i = std::distance(first, last);
  if (i > (1 << 30)) {
    internal::just1(first, i, g);
    i = (1 << 30);
  }
  internal::just2final(first, i, g);
}

// This is a template function that shuffles the elements in the range [first,
// last)
//
// It is similar to std::shuffle, but it uses a different algorithm.
//
// Performance note: This function might be slow under GCC: see shuffle_2.
template <class RandomIt, class URBG>
extern void shuffle_23456(RandomIt first, RandomIt last, URBG &&g) {
  uint64_t i = std::distance(first, last);
  if (i > (1 << 30)) {
    internal::just1(first, i, g);
    i = (1 << 30);
  }
  if (i > (1 << 19)) {
    i = internal::just2(first, i, g);
  }
  if (i > (1 << 14)) {
    i = internal::just3(first, i, g);
  }
  if (i > (1 << 11)) {
    i = internal::just4(first, i, g);
  }
  if (i > (1 << 9)) {
    i = internal::just5(first, i, g);
  }
  if (i > 6) {
    i = internal::just6(first, i, g);
  }
  if (i > 1) {
    partial_shuffle_64b(first, i, i - 1, 720, g);
  }
}

} // namespace batched_random

#endif // TEMPLATE_SHUFFLE_H
