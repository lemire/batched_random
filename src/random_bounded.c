
#include <stdint.h>
#include <stdlib.h>

#include "chacha.c"
#include "batch_shuffle_dice.c"
#include "lehmer64.h"
#include "pcg64.h"

void seed(uint64_t s) {
  lehmer64_seed(s);
  pcg64_seed(s);
  chacha8_zero(&chacha_rng, s); 
}



// Fisher-Yates shuffle, rolling one die at a time
void shuffle(uint64_t *storage, uint64_t size, uint64_t (*rng)(void)) {
  uint64_t i;
  for (i = size; i > 1; i--) {
    uint64_t nextpos = random_bounded(i, rng);
    uint64_t tmp = storage[i - 1];   // likely in cache
    uint64_t val = storage[nextpos]; // could be costly
    storage[i - 1] = val;
    storage[nextpos] = tmp; // you might have to read this store later
  }
}

// Fisher-Yates shuffle, rolling up to two dice at a time
void shuffle_batch_2(uint64_t *storage, uint64_t size, uint64_t (*rng)(void)) {
  uint64_t i = size;
  for (; i > 1 << 30; i--) {
    partial_shuffle_64b(storage, i, 1, i, rng);
  }

  // Batches of 2 for sizes up to 2^30 elements
  uint64_t bound = (uint64_t)1 << 60;
  for (; i > 1; i -= 2) {
    bound = partial_shuffle_64b(storage, i, 2, bound, rng);
  }
}

// Fisher-Yates shuffle, rolling up to six dice at a time
void shuffle_batch_23456(uint64_t *storage, uint64_t size,
                         uint64_t (*rng)(void)) {
  uint64_t i = size;
  for (; i > 1 << 30; i--) {
    partial_shuffle_64b(storage, i, 1, i, rng);
  }

  // Batches of 2 for sizes up to 2^30 elements
  uint64_t bound = (uint64_t)1 << 60;
  for (; i > 1 << 19; i -= 2) {
    bound = partial_shuffle_64b(storage, i, 2, bound, rng);
  }

  // Batches of 3 for sizes up to 2^19 elements
  bound = (uint64_t)1 << 57;
  for (; i > 1 << 14; i -= 3) {
    bound = partial_shuffle_64b(storage, i, 3, bound, rng);
  }

  // Batches of 4 for sizes up to 2^14 elements
  bound = (uint64_t)1 << 56;
  for (; i > 1 << 11; i -= 4) {
    bound = partial_shuffle_64b(storage, i, 4, bound, rng);
  }

  // Batches of 5 for sizes up to 2^11 elements
  bound = (uint64_t)1 << 55;
  for (; i > 1 << 9; i -= 5) {
    bound = partial_shuffle_64b(storage, i, 5, bound, rng);
  }

  // Batches of 6 for sizes up to 2^9 elements
  bound = (uint64_t)1 << 54;
  for (; i > 6; i -= 6) {
    bound = partial_shuffle_64b(storage, i, 6, bound, rng);
  }

  if (i > 1) {
    partial_shuffle_64b(storage, i, i - 1, 720, rng);
  }
}


// Fisher-Yates shuffle, rolling up to two dice at a time
void naive_shuffle_batch_2(uint64_t *storage, uint64_t size, uint64_t (*rng)(void)) {
  uint64_t i = size;
  for (; i > (UINT64_C(1) << 32); i--) {
    naive_partial_shuffle_64b(storage, i, 1, rng);
  }
  for (; i > 1; i -= 2) {
    naive_partial_shuffle_64b(storage, i, 2, rng);
  }
}


// Shuffle with Lehmer RNG

void shuffle_lehmer(uint64_t *storage, uint64_t size) {
  shuffle(storage, size, lehmer64);
}

void shuffle_lehmer_2(uint64_t *storage, uint64_t size) {
  shuffle_batch_2(storage, size, lehmer64);
}

void shuffle_lehmer_23456(uint64_t *storage, uint64_t size) {
  shuffle_batch_23456(storage, size, lehmer64);
}

void naive_shuffle_lehmer_2(uint64_t *storage, uint64_t size) {
  naive_shuffle_batch_2(storage, size, lehmer64);
}

// Shuffle with PCG RNG

void shuffle_pcg(uint64_t *storage, uint64_t size) {
  shuffle(storage, size, pcg64);
}

void shuffle_pcg_2(uint64_t *storage, uint64_t size) {
  shuffle_batch_2(storage, size, pcg64);
}

void shuffle_pcg_23456(uint64_t *storage, uint64_t size) {
  shuffle_batch_23456(storage, size, pcg64);
}

void naive_shuffle_pcg_2(uint64_t *storage, uint64_t size) {
  naive_shuffle_batch_2(storage, size, pcg64);
}

// Shuffle with ChaCha RNG
void shuffle_chacha(uint64_t *storage, uint64_t size) {
  shuffle(storage, size, chacha_u64_global);
}

void shuffle_chacha_2(uint64_t *storage, uint64_t size) {
  shuffle_batch_2(storage, size, chacha_u64_global);
}

void shuffle_chacha_23456(uint64_t *storage, uint64_t size) {
  shuffle_batch_23456(storage, size, chacha_u64_global);
}

void naive_shuffle_chacha_2(uint64_t *storage, uint64_t size) {
  naive_shuffle_batch_2(storage, size, chacha_u64_global);
}
// Random bounded Lehmer

uint64_t random_bounded_lehmer(uint64_t range) {
  return random_bounded(range, lehmer64);
}
