#include <stdint.h>

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
uint64_t partial_shuffle_dice_64b(
  uint64_t n,
  uint64_t k,
  uint64_t bound,
  uint64_t (*rng)(void),
  uint64_t *result
) {
  __uint128_t x;
  uint64_t r = rng();
  
  for (uint64_t i = 0; i < k; i++) {
    x = (__uint128_t) (n - i) * (__uint128_t) r;
    r = (uint64_t) x;
    result[i] = (uint64_t) (x >> 64);
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
        x = (__uint128_t) (n - i) * (__uint128_t) r;
        r = (uint64_t) x;
        result[i] = (uint64_t) (x >> 64);
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
void shuffle_17_dice_16b_interleaved(uint64_t (*rng)(void), uint16_t *result) {
  uint16_t r[4];
  uint16_t m[4] = {(1 << 10) - 1, (1 << 8) - 1, (1 << 12) - 1, (1 << 12) - 1};
  
  do {
    uint64_t bits = rng();
    for (int i = 0; i < 4; i++) {
      r[i] = (uint16_t) (bits >> (16 * i));
    }
  } while (
    ((r[0] & m[0]) == 0) || ((r[1] & m[1]) == 0) ||
    ((r[2] & m[2]) == 0) || ((r[3] & m[3]) == 0)
  );
  
  // Each column of n is a batch.
  uint16_t n[4][4] = {
    { 2,  5,  7, 12},
    { 3,  6,  8, 13},
    { 4, 16,  9, 14},
    {11, 17, 10, 15}
  };
  uint32_t x[4];
  
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      x[j] = (uint32_t) n[i][j] * (uint32_t) r[j];
    }
    // These are separate loops so the above multiplication
    // can take advantage of instruction-level parallelism.
    for (int j = 0; j < 4; j++) {
      result[n[i][j] - 2] = (uint16_t) (x[j] >> 16);
      r[j] = (uint16_t) x[j];
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
void shuffle_17_dice_16b_linear(uint64_t (*rng)(void), uint16_t *result) {
  uint16_t r[4];
  uint16_t m[4] = {(1 << 10) - 1, (1 << 8) - 1, (1 << 12) - 1, (1 << 12) - 1};
  
  do {
    uint64_t bits = rng();
    for (int i = 0; i < 4; i++) {
      r[i] = (uint16_t) (bits >> (16 * i));
    }
  } while (
    ((r[0] & m[0]) == 0) || ((r[1] & m[1]) == 0) ||
    ((r[2] & m[2]) == 0) || ((r[3] & m[3]) == 0)
  );
  
  uint16_t p[16] = {
    r[0], r[0], r[0], r[1], r[1], r[2], r[2], r[2],
    r[2], r[0], r[3], r[3], r[3], r[3], r[1], r[1]
  };
  uint16_t d[16] = {1, 2, 6, 1, 5, 1, 7, 56, 504, 24, 1, 12, 156, 2184, 30, 480};
  
  for (int i = 0; i < 16; i++) {
    p[i] *= d[i];
  }
  
  uint16_t n[16] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17};
  
  for (int i = 0; i < 16; i++) {
    uint32_t x = (uint32_t) p[i] * (uint32_t) n[i];
    result[i] = (uint16_t) (x >> 16);
  }
}