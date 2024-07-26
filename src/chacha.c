// copyright: https://github.com/nixberg/chacha-rng-c (MIT License)
// with some modifications by D. Lemire
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

#include "chacha.h"

static void chacha_init(ChaCha *rng, size_t rounds, const uint32_t seed[8], uint64_t stream) {
    rng->state[ 0] = 0x61707865;
    rng->state[ 1] = 0x3320646e;
    rng->state[ 2] = 0x79622d32;
    rng->state[ 3] = 0x6b206574;

    rng->state[ 4] = seed[0];
    rng->state[ 5] = seed[1];
    rng->state[ 6] = seed[2];
    rng->state[ 7] = seed[3];
    rng->state[ 8] = seed[4];
    rng->state[ 9] = seed[5];
    rng->state[10] = seed[6];
    rng->state[11] = seed[7];

    rng->state[12] = 0;
    rng->state[13] = 0;
    rng->state[14] = (uint32_t)stream;
    rng->state[15] = (uint32_t)(stream >> 32);

    rng->rounds = rounds;

    rng->word_index = 16;
}

void chacha8_init(ChaCha *rng, const uint32_t seed[8], uint64_t stream) {
    chacha_init(rng, 8, seed, stream);
}

void chacha8_zero(ChaCha *rng, uint64_t stream) {
    uint32_t seed[8] = { 0 };
    chacha_init(rng, 8, seed, stream);
}

void chacha12_init(ChaCha *rng, const uint32_t seed[8], uint64_t stream) {
    chacha_init(rng, 12, seed, stream);
}

void chacha12_zero(ChaCha *rng, uint64_t stream) {
    uint32_t seed[8] = { 0 };
    chacha_init(rng, 12, seed, stream);
}

void chacha20_init(ChaCha *rng, const uint32_t seed[8], uint64_t stream) {
    chacha_init(rng, 20, seed, stream);
}

void chacha20_zero(ChaCha *rng, uint64_t stream) {
    uint32_t seed[8] = { 0 };
    chacha_init(rng, 20, seed, stream);
}

uint8_t chacha_u8(ChaCha *rng) {
    return (uint8_t)chacha_u32(rng);
}

void chacha_fill_u8(ChaCha *rng, uint8_t *array, size_t count) {
    size_t tail_count = count % 4;

    for (size_t i = 0; i < (count - tail_count); i += 4) {
        uint32_t word = chacha_u32(rng);
        array[i + 0] = (uint8_t)word;
        array[i + 1] = (uint8_t)(word >> 8);
        array[i + 2] = (uint8_t)(word >> 16);
        array[i + 3] = (uint8_t)(word >> 24);
    }

    if (tail_count > 0) {
        uint32_t word = chacha_u32(rng);
        for (size_t i = tail_count; i > 0; i--) {
            array[count - i] = (uint8_t)word;
            word >>= 8;
        }
    }
}

uint16_t chacha_u16(ChaCha *rng) {
    return (uint16_t)chacha_u32(rng);
}

void chacha_fill_u16(ChaCha *rng, uint16_t *array, size_t count) {
    size_t tail_count = count % 2;

    for (size_t i = 0; i < (count - tail_count); i += 2) {
        uint32_t word = chacha_u32(rng);
        array[i + 0] = (uint16_t)word;
        array[i + 1] = (uint16_t)word >> 16;
    }

    if (tail_count > 0) {
        uint32_t word = chacha_u32(rng);
        array[count - 1] = (uint16_t)word;
    }
}

static void double_round(uint32_t state[16]);

static inline void increment_counter(ChaCha *rng) {
    rng->state[12]++;
    if (rng->state[12] == 0) {
        rng->state[13]++;
        if (rng->state[13] == 0) {
            exit(EXIT_FAILURE);
        }
    }
}

uint32_t chacha_u32(ChaCha *rng) {
    assert(rng->word_index <= 16);

    if (rng->word_index == 16) {
        for (size_t i = 0; i < 16; i++) {
            rng->working_state[i] = rng->state[i];
        }

        for (size_t i = 0; i < rng->rounds; i += 2) {
            double_round(rng->working_state);
        }

        for (size_t i = 0; i < 16; i++) {
            rng->working_state[i] += rng->state[i];
        }

        increment_counter(rng);
        rng->word_index = 0;
    }

    uint32_t result = rng->working_state[rng->word_index];

    rng->word_index++;

    return result;
}

void chacha_fill_u32(ChaCha *rng, uint32_t *array, size_t count) {
    for (size_t i = 0; i < count; i++) {
        array[i] = chacha_u32(rng);
    }
}

uint64_t chacha_u64(ChaCha *rng) {
    uint64_t lo = chacha_u32(rng);
    uint64_t hi = chacha_u32(rng);
    return (hi << 32) | lo;
}

uint64_t chacha_u64_global() {
    return chacha_u64(&chacha_rng);
}

void chacha_fill_u64(ChaCha *rng, uint64_t *array, size_t count) {
    for (size_t i = 0; i < count; i++) {
        array[i] = chacha_u64(rng);
    }
}

float chacha_f32(ChaCha *rng) {
    return (float)(chacha_u32(rng) >> 8) * 0x1p-24f;
}

void chacha_fill_f32(ChaCha *rng, float *array, size_t count) {
    for (size_t i = 0; i < count; i++) {
        array[i] = chacha_f32(rng);
    }
}

double chacha_f64(ChaCha *rng) {
    return (float)(chacha_u64(rng) >> 11) * 0x1p-53;
}

void chacha_fill_f64(ChaCha *rng, double *array, size_t count) {
    for (size_t i = 0; i < count; i++) {
        array[i] = chacha_f64(rng);
    }
}

static inline uint32_t rotated_left(uint32_t value, uint32_t count) {
    return (value << count) | (value >> (32 - count));
}

#define QUARTER_ROUND(a, b, c, d) \
    state[a] += state[b]; state[d] = rotated_left(state[d] ^ state[a], 16); \
    state[c] += state[d]; state[b] = rotated_left(state[b] ^ state[c], 12); \
    state[a] += state[b]; state[d] = rotated_left(state[d] ^ state[a],  8); \
    state[c] += state[d]; state[b] = rotated_left(state[b] ^ state[c],  7);

static inline void double_round(uint32_t state[16]) {
    QUARTER_ROUND(0, 4,  8, 12)
    QUARTER_ROUND(1, 5,  9, 13)
    QUARTER_ROUND(2, 6, 10, 14)
    QUARTER_ROUND(3, 7, 11, 15)

    QUARTER_ROUND(0, 5, 10, 15)
    QUARTER_ROUND(1, 6, 11, 12)
    QUARTER_ROUND(2, 7,  8, 13)
    QUARTER_ROUND(3, 4,  9, 14)
}
