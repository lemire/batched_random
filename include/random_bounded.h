#ifndef BATCHED_RANDOM_H
#define BATCHED_RANDOM_H
#include <stdint.h>

// call this one before calling random_bounded
void seed(uint64_t s);

// returns a random number in the range [0, range)
uint64_t random_bounded(uint64_t range);

// shuffle the storage array
void shuffle(uint64_t *storage, uint64_t size);
void shuffle_batch(uint64_t *storage, uint64_t size);
void shuffle_batch_2(uint64_t *storage, uint64_t size);

void shuffle_naive_batch(uint64_t *storage, uint64_t size);
#endif // BATCHED_RANDOM_H
