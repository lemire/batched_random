#ifndef SPLITMIX64_H
#define SPLITMIX64_H

#include <stdint.h>

inline uint64_t splitmix64_stateless(uint64_t index) {
  uint64_t z = (index * UINT64_C(0x9E3779B97F4A7C15));
  z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
  z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
  return z ^ (z >> 31);
}


// floor( ( (1+sqrt(5))/2 ) * 2**64 MOD 2**64)
#define GOLDEN_GAMMA UINT64_C(0x9E3779B97F4A7C15)

inline uint64_t splitmix64_r(uint64_t *seed) {
  uint64_t z = (*seed += GOLDEN_GAMMA);
  // David Stafford's Mix13 for MurmurHash3's 64-bit finalizer
  z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
  z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
  return z ^ (z >> 31);
}

// returns the value of splitmix64 "offset" steps from seed
inline uint64_t splitmix64_stateless_offset(uint64_t seed, uint64_t offset) {
  seed += offset*GOLDEN_GAMMA;
  return splitmix64_r(&seed);
}


#endif // SPLITMIX64_H
