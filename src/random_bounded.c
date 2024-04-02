
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "batch_shuffle_dice.c"
#include "lehmer64.h"

void seed(uint64_t s) { lehmer64_seed(s); }

uint64_t random_bounded(uint64_t range) {
  __uint128_t random64bit, multiresult;
  uint64_t leftover;
  uint64_t threshold;
  random64bit = lehmer64();
  multiresult = random64bit * range;
  leftover = (uint64_t)multiresult;
  if (leftover < range) {
    threshold = -range % range;
    while (leftover < threshold) {
      random64bit = lehmer64();
      multiresult = random64bit * range;
      leftover = (uint64_t)multiresult;
    }
  }
  return multiresult >> 64; // [0, range)
}

// Rolls a batch of fair dice with sizes n, n-1, ..., n-(k-1)
//
// Preconditions:
//   n >= k
//   bound >= n*(n-1)*...*(n-(k-1)), which must not overflow
//   rng() produces uniformly random 64-bit values
//   result has length at least k
//
// The dice rolls are put in the `result` array:
//   result[i] is an (n-i) sided die roll
//
// The return value is usable as `bound` for smaller batches of size k.
static inline uint64_t batch_random(uint64_t n, uint64_t k, uint64_t bound,
                                    uint64_t *result) {
  __uint128_t x;
  uint64_t r = lehmer64();

  for (uint64_t i = 0; i < k; i++) {
    x = (__uint128_t)(n - i) * (__uint128_t)r;
    r = (uint64_t)x;
    result[i] = (uint64_t)(x >> 64);
  }

  if (r < bound) {
    bound = n;
    for (uint64_t i = 1; i < k; i++) {
      bound *= n - i;
    }
    uint64_t t = -bound % bound;
    while (r < t) {
      r = lehmer64();
      for (uint64_t i = 0; i < k; i++) {
        x = (__uint128_t)(n - i) * (__uint128_t)r;
        r = (uint64_t)x;
        result[i] = (uint64_t)(x >> 64);
      }
    }
  }

  return bound;
}

void shuffle(uint64_t *storage, uint64_t size) {
  uint64_t i;
  for (i = size; i > 1; i--) {
    uint64_t nextpos = random_bounded(i);
    uint64_t tmp = storage[i - 1];   // likely in cache
    uint64_t val = storage[nextpos]; // could be costly
    storage[i - 1] = val;
    storage[nextpos] = tmp; // you might have to read this store later
  }
}

void shuffle_naive_batch(uint64_t *storage, uint64_t size) {
  uint64_t i;
  uint64_t nextpos, nextpos1, nextpos2, tmp, val;
  for (i = size; i > 2; i -= 2) {
    nextpos1 = random_bounded(i);
    nextpos2 = random_bounded(i - 1);

    tmp = storage[i - 1];    // likely in cache
    val = storage[nextpos1]; // could be costly
    storage[i - 1] = val;
    storage[nextpos1] = tmp; // you might have to read this store later
    tmp = storage[i - 2];    // likely in cache
    val = storage[nextpos2]; // could be costly
    storage[i - 2] = val;
    storage[nextpos2] = tmp; // you might have to read this store later
  }
  for (i = size; i > 1; i--) {
    nextpos = random_bounded(i);
    tmp = storage[i - 1];   // likely in cache
    val = storage[nextpos]; // could be costly
    storage[i - 1] = val;
    storage[nextpos] = tmp; // you might have to read this store later
  }
}

void shuffle_batch(uint64_t *storage, uint64_t size) {
  uint64_t i;
  uint64_t result[4];
  uint64_t nextpos, tmp, val;
  i = size;
  if (i <= 0x100000000) {
    for (i = size; i > 2; i -= 2) {
      batch_random(i, 2, i * (i - 1), result);
      nextpos = result[0];
      tmp = storage[i - 1];   // likely in cache
      val = storage[nextpos]; // could be costly
      storage[i - 1] = val;
      storage[nextpos] = tmp; // you might have to read this store later
      nextpos = result[1];
      tmp = storage[i - 2];   // likely in cache
      val = storage[nextpos]; // could be costly
      storage[i - 2] = val;
      storage[nextpos] = tmp; // you might have to read this store later
    }
  }

  for (; i > 1; i--) {
    nextpos = random_bounded(i);
    tmp = storage[i - 1];   // likely in cache
    val = storage[nextpos]; // could be costly
    storage[i - 1] = val;
    storage[nextpos] = tmp; // you might have to read this store later
  }
}