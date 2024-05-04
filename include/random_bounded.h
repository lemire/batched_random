#ifndef BATCHED_RANDOM_H
#define BATCHED_RANDOM_H
#include <stdint.h>

// call this one before calling random_bounded
void seed(uint64_t s);

// returns a random number in the range [0, range)
uint64_t random_bounded(uint64_t range);

// shuffle the storage array, relies on lehmer64
void shuffle(uint64_t *storage, uint64_t size);
void shuffle_batch_2(uint64_t *storage, uint64_t size);
void shuffle_batch_2_4(uint64_t *storage, uint64_t size);
void shuffle_batch_2_4_6(uint64_t *storage, uint64_t size);

// shuffle the storage array, relies on pcg64
void shuffle_pcg64(uint64_t *storage, uint64_t size);
void shuffle_batch_2_pcg64(uint64_t *storage, uint64_t size);
void shuffle_batch_2_4_pcg64(uint64_t *storage, uint64_t size);
void shuffle_batch_2_4_6_pcg64(uint64_t *storage, uint64_t size);

void shuffle_naive_batch(uint64_t *storage, uint64_t size);
#endif // BATCHED_RANDOM_H
