
/**
 * This header contains C++ templates that shuffle the elements in the range [first,
 * last) using the random number generator g. They are meant to emulate
 * the standard std::shuffle function and can often act as drop-in replacement.
 */
#ifndef TEMPLATE_SHUFFLE_H
#define TEMPLATE_SHUFFLE_H
 
#include "partial-shuffle-inl.h"

// This code is meant to look like the C++ standard library.
namespace batched_random {

// This is a template function that shuffles the elements in the range [first,
// last).
//
// It is similar to std::shuffle, but it uses a different algorithm.
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
//
// It is similar to std::shuffle, but it uses a different algorithm.
//
// Performance note: This function might be slow under GCC: see shuffle_2.
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
    partial_shuffle_64b(first, i, i - 1, 720, g);
  }
}

} // namespace batched_random

#endif // TEMPLATE_SHUFFLE_H
