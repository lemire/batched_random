
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

void pretty_print(size_t volume, size_t bytes, std::string name,
                  event_aggregate agg) {
  printf("%-40s : ", name.c_str());
  printf(" %5.2f Gi/s ", volume / agg.fastest_elapsed_ns());
  printf(" %5.2f GB/s ", bytes / agg.fastest_elapsed_ns());
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
  std::cout << "volume      : " << volume << " words" << std::endl;
  std::cout << "volume      : " << volume * sizeof(uint64_t) / 1024 / 1024.
            << " MB" << std::endl;

  size_t min_repeat = 1;
  size_t min_time_ns = 100000000;
  size_t max_repeat = 10000;
  pretty_print(volume, volume * sizeof(uint64_t), "standard shuffle",
               bench([&input]() { shuffle(input.data(), input.size()); },
                     min_repeat, min_time_ns, max_repeat));

  pretty_print(volume, volume * sizeof(uint64_t), "batch shuffle",
               bench([&input]() { shuffle_batch(input.data(), input.size()); },
                     min_repeat, min_time_ns, max_repeat));
}


int main(int argc, char **argv) {
  for(size_t i = 1<<8; i <= 1<<24; i <<= 4) {
    std::vector<uint64_t> input(i);
    bench(input);
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
