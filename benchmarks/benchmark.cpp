
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
#include "template_shuffle.h"
#include "generators.h"


void precomp_shuffle(uint64_t *storage, uint64_t size,
                     const uint32_t *precomputed) {
  uint64_t tmp, val;
  uint32_t nextpos;
  for (size_t i = size; i > 1; i--) {
    nextpos = precomputed[i];
    tmp = storage[i - 1];   // likely in cache
    val = storage[nextpos]; // could be costly
    storage[i - 1] = val;
    storage[nextpos] = tmp; // you might have to read this store later
  }
}

void pretty_print(size_t volume, size_t bytes, std::string name,
                  event_aggregate agg) {
  printf("%-45s : ", name.c_str());
  printf(" %5.2f Gi/s ", volume / agg.fastest_elapsed_ns());
  double best_speed = volume / agg.fastest_elapsed_ns();
  double avg_speed = volume / agg.elapsed_ns();
  double range =
      (best_speed - avg_speed) / avg_speed * 100.0;
  printf(" %5.2f GB/s (%2.0f %%) ", bytes / agg.fastest_elapsed_ns(), range);
  if (collector.has_events()) {
    printf(" %5.2f GHz ", agg.fastest_cycles() / agg.fastest_elapsed_ns());
    printf(" %5.2f c/b ", agg.fastest_cycles() / bytes);
    printf(" %5.2f i/b ", agg.fastest_instructions() / bytes);
    printf(" %5.2f i/c ", agg.fastest_instructions() / agg.fastest_cycles());
  }
  printf("\n");
}

void bench(std::vector<uint64_t> &input) {
  size_t volume = input.size();
  if (volume == 0) {
    return;
  }
  if (volume > 0xFFFFFFFF) {
    std::cerr << "WARNING: Volume too large for precomputed shuffle."
              << std::endl;
  }
  std::vector<uint32_t> precomputed(volume + 1);
  for (size_t i = 1; i < volume + 1; i++) {
    precomputed[i] = random_bounded(i);
  }
  std::cout << "Volume of precomputed values "
            << precomputed.size() * sizeof(uint32_t) / 1024 << " kB"
            << std::endl;

  std::random_device rd;
  std::mt19937_64 mtGenerator{rd()};
  lehmer64 lehmerGenerator{rd};

  std::cout << "volume      : " << volume << " words" << std::endl;
  std::cout << "volume      : " << volume * sizeof(uint64_t) / 1024 / 1024.
            << " MB" << std::endl;

  size_t min_repeat = 10;
  size_t min_time_ns = 100000000;
  size_t max_repeat = 100000;
  //printf("Note: The C++ shuffles use std::mt19937_64.\n");

  pretty_print(volume, volume * sizeof(uint64_t), "C++ std::shuffle (lehmer)",
               bench(
                   [&input, &lehmerGenerator]() {
                     std::shuffle(input.begin(), input.end(), lehmerGenerator);
                   },
                   min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t),
               "C++ batched_random::shuffle_2  (lehmer)",
               bench(
                   [&input, &lehmerGenerator]() {
                     batched_random::shuffle_2(input.begin(), input.end(),
                                                 lehmerGenerator);
                   },
                   min_repeat, min_time_ns, max_repeat));
  pretty_print(volume, volume * sizeof(uint64_t),
               "C++ batched_random::shuffle_2_4 (lehmer)",
               bench(
                   [&input, &lehmerGenerator]() {
                     batched_random::shuffle_2_4(input.begin(), input.end(),
                                                 lehmerGenerator);
                   },
                   min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t),
               "C++ batched_random::shuffle_2_4_6 (lehmer)",
               bench(
                   [&input, &lehmerGenerator]() {
                     batched_random::shuffle_2_4_6(input.begin(), input.end(),
                                                   lehmerGenerator);
                   },
                   min_repeat, min_time_ns, max_repeat));
  // Mersenne twister

  pretty_print(volume, volume * sizeof(uint64_t), "C++ std::shuffle (mersenne)",
               bench(
                   [&input, &mtGenerator]() {
                     std::shuffle(input.begin(), input.end(), mtGenerator);
                   },
                   min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t),
               "C++ batched_random::shuffle_2 (mersenne)",
               bench(
                   [&input, &mtGenerator]() {
                     batched_random::shuffle_2(input.begin(), input.end(),
                                                 mtGenerator);
                   },
                   min_repeat, min_time_ns, max_repeat));
  pretty_print(volume, volume * sizeof(uint64_t),
               "C++ batched_random::shuffle_2_4 (mersenne)",
               bench(
                   [&input, &mtGenerator]() {
                     batched_random::shuffle_2_4(input.begin(), input.end(),
                                                 mtGenerator);
                   },
                   min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t),
               "C++ batched_random::shuffle_2_4_6 (mersenne)",
               bench(
                   [&input, &mtGenerator]() {
                     batched_random::shuffle_2_4_6(input.begin(), input.end(),
                                                   mtGenerator);
                   },
                   min_repeat, min_time_ns, max_repeat));

  // Lehmer

  pretty_print(volume, volume * sizeof(uint64_t), "standard shuffle (lehmer)",
               bench([&input]() { shuffle(input.data(), input.size()); },
                     min_repeat, min_time_ns, max_repeat));

  pretty_print(
      volume, volume * sizeof(uint64_t), "batch shuffle 2 (lehmer)",
      bench([&input]() { shuffle_batch_2(input.data(), input.size()); },
            min_repeat, min_time_ns, max_repeat));

  pretty_print(
      volume, volume * sizeof(uint64_t), "batch shuffle 2-4 (lehmer)",
      bench([&input]() { shuffle_batch_2_4(input.data(), input.size()); },
            min_repeat, min_time_ns, max_repeat));

  pretty_print(
      volume, volume * sizeof(uint64_t), "batch shuffle 2-4-6 (lehmer)",
      bench([&input]() { shuffle_batch_2_4_6(input.data(), input.size()); },
            min_repeat, min_time_ns, max_repeat));

  // PCG

  pretty_print(volume, volume * sizeof(uint64_t), "standard shuffle (PCG64)",
               bench([&input]() { shuffle_pcg64(input.data(), input.size()); },
                     min_repeat, min_time_ns, max_repeat));

  pretty_print(
      volume, volume * sizeof(uint64_t), "batch shuffle 2  (PCG64)",
      bench([&input]() { shuffle_batch_2_pcg64(input.data(), input.size()); },
            min_repeat, min_time_ns, max_repeat));

  pretty_print(
      volume, volume * sizeof(uint64_t), "batch shuffle 2-4  (PCG64)",
      bench([&input]() { shuffle_batch_2_4_pcg64(input.data(), input.size()); },
            min_repeat, min_time_ns, max_repeat));

  pretty_print(
      volume, volume * sizeof(uint64_t), "batch shuffle 2-4-6  (PCG64)",
      bench(
          [&input]() { shuffle_batch_2_4_6_pcg64(input.data(), input.size()); },
          min_repeat, min_time_ns, max_repeat));

  // Precomputed

  pretty_print(
      volume, volume * sizeof(uint64_t), "directed_shuffle (as a reference)",
      bench(
          [&input, precomputed]() {
            precomp_shuffle(input.data(), input.size(), precomputed.data());
          },
          min_repeat, min_time_ns, max_repeat));
}

int main(int , char **) {
  seed(1234);
  // We want to make sure we extend the range far enough to see regressions
  // for large arrays, if any.
  for (size_t i = 1 << 13; i <= 1 << 18; i <<= 1) {
    std::vector<uint64_t> input(i);
    bench(input);
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
