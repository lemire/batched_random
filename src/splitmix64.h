#ifndef SPLITMIX64_H
#define SPLITMIX64_H

#include <stdint.h>

static inline uint64_t splitmix64_stateless(uint64_t index) {
  uint64_t z = (index * UINT64_C(0x9E3779B97F4A7C15));
  z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
  z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
  return z ^ (z >> 31);
}

#endif // SPLITMIX64_H