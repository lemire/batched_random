#include <stdint.h>

uint64_t random_bounded(uint64_t range, uint64_t (*rng)(void)) {
  __uint128_t random64bit, multiresult;
  uint64_t leftover;
  uint64_t threshold;
  random64bit = rng();
  multiresult = random64bit * range;
  leftover = (uint64_t)multiresult;
  if (leftover < range) {
    threshold = -range % range;
    while (leftover < threshold) {
      random64bit = rng();
      multiresult = random64bit * range;
      leftover = (uint64_t)multiresult;
    }
  }
  return (uint64_t)(multiresult >> 64); // [0, range)
}

// This is a naive batched shuffle. We generate a single random number r in n*(n-1)*...*(n-(k-1)).
// Then we get the random index as
// r % n -> pos1
// r = (r / n)
// r % (n-1) -> pos2
// r = (r / (n-1))
// ...
// r % (n-k+1) -> posk (can omit the modulo here)

inline void naive_partial_shuffle_64b(uint64_t *storage, uint64_t n, uint64_t k, uint64_t (*rng)(void)) {
  uint64_t pos1, pos2;
  uint64_t val1, val2;
  uint64_t bound = n;
  for (uint64_t i = 1; i < k; i++) {
    bound *= n - i;
  }
  // Next we generate a random integer in [0, bound)
  uint64_t r = random_bounded(bound, rng);
  for (uint64_t i = 0; i < k - 1; i++) {
    pos2 = r % (n - i);
    r /= (n - i);
    pos1 = n - i - 1;
    val1 = storage[pos1];
    val2 = storage[pos2];
    storage[pos1] = val2;
    storage[pos2] = val1;
  }
  // the last one does not need a modulo
  pos2 = r;
  pos1 = n - k;
  val1 = storage[pos1];
  val2 = storage[pos2];
  storage[pos1] = val2;
  storage[pos2] = val1;
}


// Performs k steps of a Fisher-Yates shuffle on n elements, in the array
// `storage`.
//
// Preconditions:
//   n >= k >= 1
//   bound >= n*(n-1)*...*(n-(k-1)), which must not overflow
//   rng() produces uniformly random 64-bit values
//
// The return value is usable as `bound` for smaller batches of size k.
static inline uint64_t partial_shuffle_64b(uint64_t *storage, uint64_t n, uint64_t k,
                                    uint64_t bound, uint64_t (*rng)(void)) {
  __uint128_t x;
  uint64_t r = rng();
  uint64_t pos1, pos2;
  uint64_t val1, val2;
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
      r = rng();
      for (uint64_t i = 0; i < k; i++) {
        x = (__uint128_t)(n - i) * (__uint128_t)r;
        r = (uint64_t)x;
        indexes[i] = (uint64_t)(x >> 64);
      }
    }
  }
  for (uint64_t i = 0; i < k; i++) {
    pos1 = n - i - 1;
    pos2 = indexes[i];
    val1 = storage[pos1]; // should be in cache
    val2 = storage[pos2]; // might not be in cache
    storage[pos1] = val2;
    storage[pos2] = val1; // will be read later
  }
  return bound;
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
inline uint64_t partial_shuffle_dice_64b(uint64_t n, uint64_t k, uint64_t bound,
                                         uint64_t (*rng)(void),
                                         uint64_t *result) {
  __uint128_t x;
  uint64_t r = rng();

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
      r = rng();
      for (uint64_t i = 0; i < k; i++) {
        x = (__uint128_t)(n - i) * (__uint128_t)r;
        r = (uint64_t)x;
        result[i] = (uint64_t)(x >> 64);
      }
    }
  }

  return bound;
}

// Rolls fair dice with sizes n, n-1, ..., n - (4*k - 1)
// in four interleaved batches. The first die in batch j
// has size n-j, and each subsequent die is smaller by 4
//
// Preconditions:
//   n >= 4*k
//   bound >= n*(n-4)*...*(n - 4*(k-1)), which must not overflow
//   rng() produces uniformly random 64-bit values
//   result has length at least 4*k
//
// The dice rolls are put in the `result` array:
//   result[i] is an (n-i) sided die roll
//
// The return value is usable as `bound` with the same k and smaller n
inline uint64_t partial_shuffle_dice_64b_interleaved_4x(uint64_t n, uint64_t k,
                                                        uint64_t bound,
                                                        uint64_t (*rng)(void),
                                                        uint64_t *result) {
  __uint128_t x;
  uint64_t r[4];

  for (int j = 0; j < 4; j++) {
    r[j] = rng();
  }

  for (uint64_t i = 0; i < k; i++) {
    for (uint64_t j = 0; j < 4; j++) {
      x = (__uint128_t)(n - 4 * i - j) * (__uint128_t)r[j];
      r[j] = (uint64_t)x;
      result[4 * i + j] = (uint64_t)(x >> 64);
    }
  }

  for (uint64_t j = 0; j < 4; j++) {
    if (r[j] < bound) {
      uint64_t m = n - j;
      bound = m;
      for (uint64_t i = 1; i < k; i++) {
        bound *= m - 4 * i;
      }
      uint64_t t = -bound % bound;
      while (r[j] < t) {
        r[j] = rng();
        for (uint64_t i = 0; i < k; i++) {
          x = (__uint128_t)(m - 4 * i) * (__uint128_t)r[j];
          r[j] = (uint64_t)x;
          result[4 * i + j] = (uint64_t)(x >> 64);
        }
      }
    }
  }

  return bound;
}

// Rolls a batch of fair dice with sizes 2, 3, ..., 17
//
// Preconditions:
//   rng() produces uniformly random 64-bit values
//   result has length at least 16
//
// The dice rolls are put in the `result` array:
//   result[i] is an (i+2) sided die roll
inline void shuffle_17_dice_16b_interleaved(uint64_t (*rng)(void),
                                            uint16_t *result) {
  uint16_t r[4];
  uint16_t m[4] = {(1 << 10) - 1, (1 << 8) - 1, (1 << 12) - 1, (1 << 12) - 1};

  do {
    uint64_t bits = rng();
    for (int i = 0; i < 4; i++) {
      r[i] = (uint16_t)(bits >> (16 * i));
    }
  } while (((r[0] & m[0]) == 0) || ((r[1] & m[1]) == 0) ||
           ((r[2] & m[2]) == 0) || ((r[3] & m[3]) == 0));

  // Each column of n is a batch.
  uint16_t n[4][4] = {
      {2, 5, 7, 12}, {3, 6, 8, 13}, {4, 16, 9, 14}, {11, 17, 10, 15}};
  uint32_t x[4];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      x[j] = (uint32_t)n[i][j] * (uint32_t)r[j];
    }
    // These are separate loops so the above multiplication
    // can take advantage of instruction-level parallelism.
    for (int j = 0; j < 4; j++) {
      result[n[i][j] - 2] = (uint16_t)(x[j] >> 16);
      r[j] = (uint16_t)x[j];
    }
  }
}

// Rolls a batch of fair dice with sizes 2, 3, ..., 17
//
// Preconditions:
//   rng() produces uniformly random 64-bit values
//   result has length at least 16
//
// The dice rolls are put in the `result` array:
//   result[i] is an (i+2) sided die roll
inline void shuffle_17_dice_16b_linear(uint64_t (*rng)(void),
                                       uint16_t *result) {
  uint16_t r[4];
  uint16_t m[4] = {(1 << 10) - 1, (1 << 8) - 1, (1 << 12) - 1, (1 << 12) - 1};

  do {
    uint64_t bits = rng();
    for (int i = 0; i < 4; i++) {
      r[i] = (uint16_t)(bits >> (16 * i));
    }
  } while (((r[0] & m[0]) == 0) || ((r[1] & m[1]) == 0) ||
           ((r[2] & m[2]) == 0) || ((r[3] & m[3]) == 0));

  uint16_t p[16] = {r[0], r[0], r[0], r[1], r[1], r[2], r[2], r[2],
                    r[2], r[0], r[3], r[3], r[3], r[3], r[1], r[1]};
  uint16_t d[16] = {1,   2,  6, 1,  5,   1,    7,  56,
                    504, 24, 1, 12, 156, 2184, 30, 480};

  for (int i = 0; i < 16; i++) {
    p[i] *= d[i];
  }

  uint16_t n[16] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};

  for (int i = 0; i < 16; i++) {
    uint32_t x = (uint32_t)p[i] * (uint32_t)n[i];
    result[i] = (uint16_t)(x >> 16);
  }
}
