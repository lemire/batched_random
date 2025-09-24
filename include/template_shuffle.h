
/**
 * This header contains C++ templates that shuffle the elements in the range [first,
 * last) using the random number generator g. They are meant to emulate
 * the standard std::shuffle function and can often act as drop-in replacement.
 */
#ifndef TEMPLATE_SHUFFLE_H
#define TEMPLATE_SHUFFLE_H
 
#include "partial-shuffle-inl.h"
#include <iterator>
#include <concepts>
#include <type_traits>

// This code is meant to look like the C++ standard library.
namespace batched_random {

// Function shuffle_2p: Shuffles the elements in the range [first, last) using batches of 2
template <class RandomIt, class URBG>
void shuffle_2p(RandomIt first, RandomIt last, URBG&& g) {
    static_assert(std::is_same<typename std::iterator_traits<RandomIt>::iterator_category, std::random_access_iterator_tag>::value, "RandomIt must be a random access iterator");
    static_assert(std::is_same<typename std::decay_t<URBG>::result_type, uint64_t>::value, "result_type must be uint64_t");
    
    uint64_t i = std::distance(first, last);
    
    // Phase 1: Single-element shuffles for sizes above 2^30
    for (; i > 1ULL << 30; i--) {
        __uint128_t x;
        uint64_t r = g();
        x = (__uint128_t)(i) * (__uint128_t)r;
        uint64_t index = (uint64_t)(x >> 64);
        uint64_t bound = i;
        [[unlikely]] if (r < bound) {
            uint64_t t = -bound % bound;
            while (r < t) {
                r = g();
                x = (__uint128_t)(i) * (__uint128_t)r;
                index = (uint64_t)(x >> 64);
            }
        }
        std::iter_swap(first + (i - 1), first + index);
    }
    
    // Phase 2: Batches of 2 for sizes up to 2^30 elements
    uint64_t bound = (uint64_t)1 << 60;
    for (; i > 1; i -= 2) {
        __uint128_t x;
        uint64_t r = g();
        uint64_t index0, index1;
        // Generate indices for the batch of 2
        x = (__uint128_t)(i - 0) * (__uint128_t)r;
        r = (uint64_t)x;
        index0 = (uint64_t)(x >> 64);
        x = (__uint128_t)(i - 1) * (__uint128_t)r;
        r = (uint64_t)x;
        index1 = (uint64_t)(x >> 64);
        // Bound checking
        [[unlikely]] if (r < bound) {
            uint64_t new_bound = i;
            new_bound *= (i - 1);
            uint64_t t = -new_bound % new_bound;
            while (r < t) {
                r = g();
                x = (__uint128_t)(i - 0) * (__uint128_t)r;
                r = (uint64_t)x;
                index0 = (uint64_t)(x >> 64);
                x = (__uint128_t)(i - 1) * (__uint128_t)r;
                r = (uint64_t)x;
                index1 = (uint64_t)(x >> 64);
            }
            bound = new_bound;
        }
        // Perform swaps for the batch of 2
        std::iter_swap(first + (i - 1), first + index0);
        std::iter_swap(first + (i - 2), first + index1);
    }
}

// Function shuffle_24: Shuffles the elements in the range [first, last) using batches of 2 to 6
template <class RandomIt, class URBG>
void shuffle_24(RandomIt first, RandomIt last, URBG&& g) {
    static_assert(std::is_same<typename std::iterator_traits<RandomIt>::iterator_category, std::random_access_iterator_tag>::value, "RandomIt must be a random access iterator");
    static_assert(std::is_same<typename std::decay_t<URBG>::result_type, uint64_t>::value, "result_type must be uint64_t");
    
    uint64_t i = std::distance(first, last);

    // Phase 1: Single-element shuffles for sizes above 2^30
    for (; i > 1ULL << 30; i--) {
        __uint128_t x;
        uint64_t r = g();
        x = (__uint128_t)(i) * (__uint128_t)r;
        uint64_t index = (uint64_t)(x >> 64);
        uint64_t bound = i;
        [[unlikely]] if (r < bound) {
            uint64_t t = -bound % bound;
            while (r < t) {
                r = g();
                x = (__uint128_t)(i) * (__uint128_t)r;
                index = (uint64_t)(x >> 64);
            }
        }
        std::iter_swap(first + (i - 1), first + index);
    }

    // Phase 2: Batches of 2 for sizes up to 2^30 elements
    uint64_t bound = (uint64_t)1 << 60;
    for (; i > 1ULL << 14; i -= 2) {
        __uint128_t x;
        uint64_t r = g();
        uint64_t index0, index1;
        x = (__uint128_t)(i - 0) * (__uint128_t)r;
        r = (uint64_t)x;
        index0 = (uint64_t)(x >> 64);
        x = (__uint128_t)(i - 1) * (__uint128_t)r;
        r = (uint64_t)x;
        index1 = (uint64_t)(x >> 64);
        [[unlikely]] if (r < bound) {
            uint64_t new_bound = i;
            new_bound *= (i - 1);
            uint64_t t = -new_bound % new_bound;
            while (r < t) {
                r = g();
                x = (__uint128_t)(i - 0) * (__uint128_t)r;
                r = (uint64_t)x;
                index0 = (uint64_t)(x >> 64);
                x = (__uint128_t)(i - 1) * (__uint128_t)r;
                r = (uint64_t)x;
                index1 = (uint64_t)(x >> 64);
            }
            bound = new_bound;
        }
        std::iter_swap(first + (i - 1), first + index0);
        std::iter_swap(first + (i - 2), first + index1);
    }

    // Phase 4: Batches of 4 for sizes up to 2^14 elements
    bound = (uint64_t)1 << 56;
    for (; i > 4; i -= 4) {
        __uint128_t x;
        uint64_t r = g();
        uint64_t index0, index1, index2, index3;
        x = (__uint128_t)(i - 0) * (__uint128_t)r;
        r = (uint64_t)x;
        index0 = (uint64_t)(x >> 64);
        x = (__uint128_t)(i - 1) * (__uint128_t)r;
        r = (uint64_t)x;
        index1 = (uint64_t)(x >> 64);
        x = (__uint128_t)(i - 2) * (__uint128_t)r;
        r = (uint64_t)x;
        index2 = (uint64_t)(x >> 64);
        x = (__uint128_t)(i - 3) * (__uint128_t)r;
        r = (uint64_t)x;
        index3 = (uint64_t)(x >> 64);
        [[unlikely]] if (r < bound) {
            uint64_t new_bound = i;
            for (uint64_t j = 1; j < 4; ++j) {
                new_bound *= (i - j);
            }
            uint64_t t = -new_bound % new_bound;
            while (r < t) {
                r = g();
                x = (__uint128_t)(i - 0) * (__uint128_t)r;
                r = (uint64_t)x;
                index0 = (uint64_t)(x >> 64);
                x = (__uint128_t)(i - 1) * (__uint128_t)r;
                r = (uint64_t)x;
                index1 = (uint64_t)(x >> 64);
                x = (__uint128_t)(i - 2) * (__uint128_t)r;
                r = (uint64_t)x;
                index2 = (uint64_t)(x >> 64);
                x = (__uint128_t)(i - 3) * (__uint128_t)r;
                r = (uint64_t)x;
                index3 = (uint64_t)(x >> 64);
            }
            bound = new_bound;
        }
        std::iter_swap(first + (i - 1), first + index0);
        std::iter_swap(first + (i - 2), first + index1);
        std::iter_swap(first + (i - 3), first + index2);
        std::iter_swap(first + (i - 4), first + index3);
    }

    // Final cleanup: Handle remaining elements (i-1 swaps if i > 1)
    if (i > 1) {
        __uint128_t x;
        uint64_t r = g();
        uint64_t k = i - 1;
        uint64_t indexes[4];
        for (uint64_t j = 0; j < k; ++j) {
            x = (__uint128_t)(i - j) * (__uint128_t)r;
            r = (uint64_t)x;
            indexes[j] = (uint64_t)(x >> 64);
        }
        if (r < 720) {
            uint64_t new_bound = i;
            for (uint64_t j = 1; j < k; ++j) {
                new_bound *= (i - j);
            }
            uint64_t t = -new_bound % new_bound;
            while (r < t) {
                r = g();
                for (uint64_t j = 0; j < k; ++j) {
                    x = (__uint128_t)(i - j) * (__uint128_t)r;
                    r = (uint64_t)x;
                    indexes[j] = (uint64_t)(x >> 64);
                }
            }
        }
        for (uint64_t j = 0; j < k; ++j) {
            std::iter_swap(first + (i - j - 1), first + indexes[j]);
        }
    }
}

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



template <class RandomIt, class URBG>
void shuffle_23456p(RandomIt first, RandomIt last, URBG &&g) {
    // Ensure the random number generator produces 64-bit unsigned integers
    static_assert(std::is_same<typename std::remove_reference<URBG>::type::result_type, uint64_t>::value, "result_type must be uint64_t");
    
    // Calculate the number of elements to shuffle
    uint64_t i = std::distance(first, last);

    // Lambda to perform a partial Fisher-Yates shuffle for k elements out of n
    // Parameters:
    // - storage: Iterator to the start of the array
    // - n: Total number of elements in the current shuffle range
    // - k: Number of elements to shuffle in this batch (k <= 7)
    // - bound: Upper bound for uniform random number generation to avoid bias
    // Returns: Updated bound for the next iteration
    auto partial_shuffle = [&](RandomIt storage, uint64_t n, uint64_t k, uint64_t bound) -> uint64_t {
        // Use 128-bit arithmetic to avoid overflow in random number scaling
        __uint128_t x;
        // Get a random 64-bit value from the generator
        uint64_t r = g();
        // Store indices for swapping (k <= 7, so fixed-size array is safe)
        uint64_t indexes[7];
        // Generate k random indices using the division method
        for (uint64_t j = 0; j < k; j++) {
            // Scale random number to select an index in [0, n-j)
            x = (__uint128_t)(n - j) * (__uint128_t)r;
            r = (uint64_t)x; // Lower 64 bits for next iteration
            indexes[j] = (uint64_t)(x >> 64); // Upper 64 bits give the index
        }
        // Check for bias in random number generation
        [[unlikely]] if (r < bound) {
            // Recalculate bound as n * (n-1) * ... * (n-(k-1))
            bound = n;
            for (uint64_t j = 1; j < k; j++) {
                bound *= n - j;
            }
            // Compute threshold to reject biased random numbers
            uint64_t t = -bound % bound;
            // Regenerate random numbers until unbiased
            while (r < t) {
                r = g();
                for (uint64_t j = 0; j < k; j++) {
                    x = (__uint128_t)(n - j) * (__uint128_t)r;
                    r = (uint64_t)x;
                    indexes[j] = (uint64_t)(x >> 64);
                }
            }
        }
        // Perform swaps to shuffle k elements
        for (uint64_t j = 0; j < k; j++) {
            std::iter_swap(storage + n - j - 1, storage + indexes[j]);
        }
        return bound;
    };

    // Process large arrays (above 2^30 elements) one element at a time
    for (; i > 1 << 30; i--) {
        partial_shuffle(first, i, 1, i);
    }
    // Batches of 2 for sizes up to 2^30 elements
    uint64_t bound = (uint64_t)1 << 60;
    for (; i > 1 << 19; i -= 2) {
        bound = partial_shuffle(first, i, 2, bound);
    }
    // Batches of 3 for sizes up to 2^19 elements
    bound = (uint64_t)1 << 57;
    for (; i > 1 << 14; i -= 3) {
        bound = partial_shuffle(first, i, 3, bound);
    }
    // Batches of 4 for sizes up to 2^14 elements
    bound = (uint64_t)1 << 56;
    for (; i > 1 << 11; i -= 4) {
        bound = partial_shuffle(first, i, 4, bound);
    }
    // Batches of 5 for sizes up to 2^11 elements
    bound = (uint64_t)1 << 55;
    for (; i > 1 << 9; i -= 5) {
        bound = partial_shuffle(first, i, 5, bound);
    }
    // Batches of 6 for sizes up to 2^9 elements
    bound = (uint64_t)1 << 54;
    for (; i > 6; i -= 6) {
        bound = partial_shuffle(first, i, 6, bound);
    }
    // Handle remaining elements (2 to 6) in a single batch
    if (i > 1) {
        partial_shuffle(first, i, i - 1, 720);
    }
}

} // namespace batched_random

#endif // TEMPLATE_SHUFFLE_H
