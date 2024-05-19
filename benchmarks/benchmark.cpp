
#include "performancecounters/benchmarker.h"
#include <algorithm>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <vector>
extern "C" {
#include "random_bounded.h"
}
#include "generators.h"
#include "template_shuffle.h"

void precomp_shuffle(uint64_t *storage, uint64_t size,
                     const uint32_t *precomputed) {
  uint64_t tmp, val;
  uint32_t nextpos;
  for (size_t i = size - 1; i > 0; i--) {
    nextpos = precomputed[i];
    tmp = storage[i];   // likely in cache
    val = storage[nextpos]; // could be costly
    storage[i] = val;
    storage[nextpos] = tmp; // you might have to read this store later
  }
}

void pretty_print(size_t volume, size_t bytes, std::string name,
                  event_aggregate agg) {
  printf("%-45s : ", name.c_str());
  printf(" %5.2f Gi/s ", volume / agg.fastest_elapsed_ns());
  double best_speed = volume / agg.fastest_elapsed_ns();
  double avg_speed = volume / agg.elapsed_ns();
  double range = (best_speed - avg_speed) / avg_speed * 100.0;
  printf(" %5.2f GB/s best, %5.2f GB/s average, (%2.0f %%) ",
         bytes / agg.fastest_elapsed_ns(), bytes / agg.elapsed_ns(), range);
  if (collector.has_events()) {
    printf(" %5.2f GHz ", agg.fastest_cycles() / agg.fastest_elapsed_ns());
    printf(" %5.2f c/b ", agg.fastest_cycles() / bytes);
    printf(" %5.2f i/b ", agg.fastest_instructions() / bytes);
    printf(" %5.2f i/c ", agg.fastest_instructions() / agg.fastest_cycles());
  }
  printf("\n");
}

void bench(size_t size, bool include_cpp) {
  constexpr size_t min_volume = 4096;
  if (size == 0) {
    return;
  }
  size_t volume = size;
  if (size < min_volume) {
    volume *= min_volume / size;
  }
  std::vector<uint64_t> input(volume);
  std::random_device rd;

  if (size > 0xFFFFFFFF) {
    std::cerr << "WARNING: Volume too large for precomputed shuffle."
              << std::endl;
  }
  std::vector<uint32_t> precomputed(volume);
  for (size_t i = 0; i < volume; i++) {
    input[i] = i;
    uint64_t bound = (i % size) + 1;
    precomputed[i] = random_bounded_lehmer(bound);
  }

  std::cout << "Size of precomputed values "
            << size * sizeof(uint32_t) / 1024 << " kB"
            << std::endl;
  std::cout << "Size of shuffle      : " << size << " words" << std::endl;
  std::cout << "Size of shuffle      : " << size * sizeof(uint64_t) / 1024 / 1024.
            << " MB" << std::endl;

  size_t min_repeat = 10;
  size_t min_time_ns = 100000000;
  size_t max_repeat = 100000;

  if (size < volume) {
    printf("inner repeat: %zu\n", volume / size);
  }

  if (include_cpp) {
    lehmer64 lehmerGenerator{rd()};
    std::mt19937_64 mtGenerator{rd()};

    // C++ Lehmer

    pretty_print(volume, volume * sizeof(uint64_t),
                 "C++ std::shuffle (lehmer)",
                 bench(
                     [&input, &lehmerGenerator, size]() {
                       for (auto t = input.begin(); t < input.end(); t += size) {
                         std::shuffle(t, t + size, lehmerGenerator);
                       }
                     },
                     min_repeat, min_time_ns, max_repeat));

    pretty_print(volume, volume * sizeof(uint64_t),
                 "C++ shuffle 2 (lehmer)",
                 bench(
                     [&input, &lehmerGenerator, size]() {
                       for (auto t = input.begin(); t < input.end(); t += size) {
                         batched_random::shuffle_2(t, t + size, lehmerGenerator);
                       }
                     },
                     min_repeat, min_time_ns, max_repeat));

    pretty_print(volume, volume * sizeof(uint64_t),
                 "C++ shuffle 2-6 (lehmer)",
                 bench(
                     [&input, &lehmerGenerator, size]() {
                       for (auto t = input.begin(); t < input.end(); t += size) {
                         batched_random::shuffle_23456(t, t + size, lehmerGenerator);
                       }
                     },
                     min_repeat, min_time_ns, max_repeat));

    // C++ Mersenne twister

    pretty_print(volume, volume * sizeof(uint64_t),
                 "C++ std::shuffle (mersenne)",
                 bench(
                     [&input, &mtGenerator, size]() {
                       for (auto t = input.begin(); t < input.end(); t += size) {
                         std::shuffle(t, t + size, mtGenerator);
                       }
                     },
                     min_repeat, min_time_ns, max_repeat));

    pretty_print(volume, volume * sizeof(uint64_t),
                 "C++ shuffle 2 (mersenne)",
                 bench(
                     [&input, &mtGenerator, size]() {
                       for (auto t = input.begin(); t < input.end(); t += size) {
                         batched_random::shuffle_2(t, t + size, mtGenerator);
                       }
                     },
                     min_repeat, min_time_ns, max_repeat));

    pretty_print(volume, volume * sizeof(uint64_t),
                 "C++ shuffle 2-6 (mersenne)",
                 bench(
                     [&input, &mtGenerator, size]() {
                       for (auto t = input.begin(); t < input.end(); t += size) {
                         batched_random::shuffle_23456(t, t + size, mtGenerator);
                       }
                     },
                     min_repeat, min_time_ns, max_repeat));
  }

  // Lehmer

  pretty_print(volume, volume * sizeof(uint64_t),
               "standard shuffle (lehmer)",
               bench(
                   [&input, size, volume]() {
                     for (size_t t = 0; t < volume; t += size) {
                       shuffle_lehmer(input.data() + t, size);
                     }
                   },
                   min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t),
               "batch shuffle 2 (lehmer)",
               bench(
                   [&input, size, volume]() {
                     for (size_t t = 0; t < volume; t += size) {
                       shuffle_lehmer_2(input.data() + t, size);
                     }
                   },
                   min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t),
               "batch shuffle 2-6 (lehmer)",
               bench(
                   [&input, size, volume]() {
                     for (size_t t = 0; t < volume; t += size) {
                       shuffle_lehmer_23456(input.data() + t, size);
                     }
                   },
                   min_repeat, min_time_ns, max_repeat));

  // PCG

  pretty_print(volume, volume * sizeof(uint64_t),
               "standard shuffle (PCG)",
               bench(
                   [&input, size, volume]() {
                     for (size_t t = 0; t < volume; t += size) {
                       shuffle_pcg(input.data() + t, size);
                     }
                   },
                   min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t),
               "batch shuffle 2 (PCG)",
               bench(
                   [&input, size, volume]() {
                     for (size_t t = 0; t < volume; t += size) {
                       shuffle_pcg_2(input.data() + t, size);
                     }
                   },
                   min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t),
               "batch shuffle 2-6 (PCG)",
               bench(
                   [&input, size, volume]() {
                     for (size_t t = 0; t < volume; t += size) {
                       shuffle_pcg_23456(input.data() + t, size);
                     }
                   },
                   min_repeat, min_time_ns, max_repeat));

  // Precomputed

  pretty_print(volume, volume * sizeof(uint64_t),
               "directed_shuffle (as a reference)",
               bench(
                   [&input, precomputed, size, volume]() {
                     for (size_t t = 0; t < volume; t += size) {
                       precomp_shuffle(input.data() + t, size,
                                       precomputed.data() + t);
                     }
                   },
                   min_repeat, min_time_ns, max_repeat));
}

int main(int argc, char **argv) {
  seed(1234);
  bool include_cpp = false;
  if (argc > 1) {
    if (std::string(argv[1]) == "--cpp") {
      include_cpp = true;
    }
  }

  // We want to make sure we extend the range far enough to see regressions
  // for large arrays, if any.
  for (size_t i = 1 << 9; i <= 1 << 20; i <<= 1) {
    bench(i, include_cpp);
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
