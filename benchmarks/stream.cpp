
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
std::vector<uint32_t> precomputed;
void precomp_shuffle(uint64_t *storage, uint64_t size) {
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
  (void)bytes;
  (void)name;
  printf(" %5.2f  ", agg.fastest_elapsed_ns() / volume);
  printf(" %5.2f  ", agg.elapsed_ns() / volume);
  printf("\t");
  fflush(stdout);
}

using shuffle_function = void (*)(uint64_t *, uint64_t);

struct named_function {
  std::string name;
  shuffle_function function;
};

named_function func[] = {
    {"precomp_shuffle", precomp_shuffle},
    {"shuffle", shuffle},
    {"shuffle_batch_2", shuffle_batch_2},
    {"shuffle_batch_2_4", shuffle_batch_2_4},
    {"shuffle_batch_2_4_6", shuffle_batch_2_4_6},
    {"shuffle_pcg64", shuffle_pcg64},
    {"shuffle_batch_2_pcg64", shuffle_batch_2_pcg64},
    {"shuffle_batch_2_4_pcg64", shuffle_batch_2_4_pcg64},
    {"shuffle_batch_2_4_6_pcg64", shuffle_batch_2_4_6_pcg64}};

using cpp_shuffle_function = void (*)(std::vector<uint64_t>::iterator,
                                      std::vector<uint64_t>::iterator,
                                      std::mt19937_64 &&);
struct named_cpp_function {
  std::string name;
  cpp_shuffle_function function;
};
named_cpp_function cppfunc[] = {
    {"std::shuffle",
     std::shuffle<std::vector<uint64_t>::iterator, std::mt19937_64>},
    {"batched_random::shuffle_2",
     batched_random::shuffle_2<std::vector<uint64_t>::iterator,
                               std::mt19937_64>},
    {"batched_random::shuffle_2_4",
     batched_random::shuffle_2_4<std::vector<uint64_t>::iterator,
                                 std::mt19937_64>},
    {"batched_random::shuffle_2_4_6",
     batched_random::shuffle_2_4_6<std::vector<uint64_t>::iterator,
                                   std::mt19937_64>},

};

void bench_line(std::vector<uint64_t> &input) {
  size_t volume = input.size();
  printf("%zu\t\t", volume);
  precomputed.resize(volume + 1);
  for (size_t i = 1; i < volume + 1; i++) {
    precomputed[i] = random_bounded(i);
  }
  std::random_device rd;
  std::mt19937_64 mtGenerator{rd()};

  for (auto &f : cppfunc) {
    pretty_print(volume, volume * sizeof(uint64_t), f.name,
                 bench([&input, &f, &mtGenerator]() {
                   f.function(input.begin(), input.end(),
                              std::mt19937_64(mtGenerator));
                 }));
  }
  for (auto &f : func) {
    pretty_print(
        volume, volume * sizeof(uint64_t), f.name,
        bench([&input, &f]() { f.function(input.data(), input.size()); }));
  }
}

void bench_table(size_t start, size_t end, size_t lines) {
  double b = pow(double(end)/start, 1.0/lines);
  printf("# for each scheme, we give the best time/item and the average "
         "time/item in ns \n");
  printf("# Volume\t");
  for (auto &f : cppfunc) {
    printf("\t%s", f.name.c_str());
  }
  for (auto &f : func) {
    printf("\t%s", f.name.c_str());
  }
  printf("\n");
  for (double i = start; i <= end; i *= b) {
    std::vector<uint64_t> input(round(i));
    bench_line(input);
    std::cout << std::endl;
  }
}

int main(int , char **) {
  seed(1234);
  bench_table(1 << 8, 1 << 16, 16);
  return EXIT_SUCCESS;
}
