
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


void precomp_shuffle(uint64_t *storage, uint64_t size, const uint64_t *precomputed) {
  uint64_t nextpos, tmp, val;
  for (size_t i = size; i > 1; i--) {
    nextpos = precomputed[i];
    tmp = storage[i - 1];   // likely in cache
    val = storage[nextpos]; // could be costly
    storage[i - 1] = val;
    storage[nextpos] = tmp; // you might have to read this store later
  }
}
void pretty_print(size_t volume, size_t bytes, std::string name,
                  event_aggregate agg, bool display = true) {
  printf("%-40s : ", name.c_str());
  printf(" %5.2f Gi/s ", volume / agg.fastest_elapsed_ns());
  double range = (agg.elapsed_ns() - agg.fastest_elapsed_ns())/agg.elapsed_ns() * 100.0;
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
  std::vector<uint64_t> precomputed(volume+1);
  for (size_t i = 1; i < volume+1; i++) {
    precomputed[i] = random_bounded(i);
  }
  if (volume == 0) {
    return;
  }
  std::cout << "volume      : " << volume << " words" << std::endl;
  std::cout << "volume      : " << volume * sizeof(uint64_t) / 1024 / 1024.
            << " MB" << std::endl;

  size_t min_repeat = 10;
  size_t min_time_ns = 10000000000;
  size_t max_repeat = 100000;
  pretty_print(volume, volume * sizeof(uint64_t), "standard shuffle",
               bench([&input]() { shuffle(input.data(), input.size()); },
                     min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t), "batch shuffle",
               bench([&input]() { shuffle_batch(input.data(), input.size()); },
                     min_repeat, min_time_ns, max_repeat));
  pretty_print(volume, volume * sizeof(uint64_t), "directed_shuffle (as a reference)",
               bench([&input,precomputed]() { precomp_shuffle(input.data(), input.size(), precomputed.data()); },
                     min_repeat, min_time_ns, max_repeat));

}


int main(int argc, char **argv) {
  for(size_t i = 1<<8; i <= 1<<24; i <<= 2) {
    std::vector<uint64_t> input(i);
    bench(input);
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
