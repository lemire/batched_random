#ifndef BATCHED_RANDOM_H
#define BATCHED_RANDOM_H
#include <stdint.h>

// call this one before calling random_bounded
void seed(uint64_t s);

// returns a random number in the range [0, range)
uint64_t random_bounded(uint64_t range, uint64_t (*rng)(void));
uint64_t random_bounded_lehmer(uint64_t range);

// shuffle the storage array
void shuffle(uint64_t *storage, uint64_t size, uint64_t (*rng)(void));
void shuffle_batch_2(uint64_t *storage, uint64_t size, uint64_t (*rng)(void));
void shuffle_batch_23456(uint64_t *storage, uint64_t size,
                         uint64_t (*rng)(void));

// shuffle with lehmer rng
void shuffle_lehmer(uint64_t *storage, uint64_t size);
void shuffle_lehmer_2(uint64_t *storage, uint64_t size);
void shuffle_lehmer_23456(uint64_t *storage, uint64_t size);

// shuffle with pcg rng
void shuffle_pcg(uint64_t *storage, uint64_t size);
void shuffle_pcg_2(uint64_t *storage, uint64_t size);
void shuffle_pcg_23456(uint64_t *storage, uint64_t size);

#endif // BATCHED_RANDOM_H
