/***
 * This header contains function declarations for C array shuffling functions.
 * It can be called by C code.
 */
#ifndef BATCHED_RANDOM_H
#define BATCHED_RANDOM_H
#include <stdint.h>

// call this one before calling random_bounded and other shuffling functions.
void seed(uint64_t s);


// shuffle the storage array, you need to provide your own random number
// generator (rng)
void shuffle(uint64_t *storage, uint64_t size, uint64_t (*rng)(void));
void shuffle_batch_2(uint64_t *storage, uint64_t size, uint64_t (*rng)(void));
void shuffle_batch_23456(uint64_t *storage, uint64_t size,
                         uint64_t (*rng)(void));
void naive_shuffle_batch_2(uint64_t *storage, uint64_t size, uint64_t (*rng)(void));

// shuffle with lehmer rng
void shuffle_lehmer(uint64_t *storage, uint64_t size);
void shuffle_lehmer_2(uint64_t *storage, uint64_t size);
void shuffle_lehmer_23456(uint64_t *storage, uint64_t size);
void naive_shuffle_lehmer_2(uint64_t *storage, uint64_t size);

// shuffle with pcg64 rng
void shuffle_pcg(uint64_t *storage, uint64_t size);
void shuffle_pcg_2(uint64_t *storage, uint64_t size);
void shuffle_pcg_23456(uint64_t *storage, uint64_t size);
void naive_shuffle_pcg_2(uint64_t *storage, uint64_t size);


// shuffle with chacha rng
void shuffle_chacha(uint64_t *storage, uint64_t size);
void shuffle_chacha_2(uint64_t *storage, uint64_t size);
void shuffle_chacha_23456(uint64_t *storage, uint64_t size);
void naive_shuffle_chacha_2(uint64_t *storage, uint64_t size);


// returns a random number in the range [0, range)
uint64_t random_bounded_lehmer(uint64_t range);

#endif // BATCHED_RANDOM_H
