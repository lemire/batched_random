#include <array>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <limits>

extern "C" {
#include "random_bounded.h"
}
#include "template_shuffle.h"

/***
 * How do we test a shuffle function?
 * There are many tests that one could apply to a shuffle function.
 */
using shuffle_function = void (*)(uint64_t *, uint64_t);

template <class function_type>
bool everyone_can_move_everywhere(const function_type &function) {
  constexpr size_t size = 512;
  uint64_t input[size];
  std::bitset<size> bits[size];
  for (size_t trial = 0; trial < size * size; trial++) {
    // We always start from the same input.
    std::iota(input, input + size, 0);
    // We shuffle:
    function(input, size);
    // Mark that at position i we found value input[i].
    for (size_t i = 0; i < size; i++) {
      bits[i][input[i]] = 1;
    }
  }
  for (const std::bitset<size> &b : bits) {
    if (!b.all()) {
      return false;
    }
  }
  return true;
}

template <class function_type>
bool uniformity_test(const function_type &function) {
  constexpr size_t size = 512;
  uint64_t input[size];
  std::array<size_t, size> bits[size]{};
  size_t volume = size * size;
  for (size_t trial = 0; trial < volume; trial++) {
    // We always start from the same input.
    std::iota(input, input + size, 0);
    // We shuffle:
    function(input, size);
    // Mark that at position i we found value input[i].
    for (size_t i = 0; i < size; i++) {
      bits[i][input[i]] += 1;
    }
  }
  size_t overall_min {std::numeric_limits<size_t>::max() };
  size_t overall_max = 0;
  size_t average = 0;

  for (const std::array<size_t, size> &b : bits) {
    average += std::accumulate(b.begin(), b.end(), 0);
    size_t max_value = *std::max_element(b.begin(), b.end());
    size_t min_value = *std::min_element(b.begin(), b.end());
    if (max_value > overall_max) {
      overall_max = max_value;
    }
    if (min_value < overall_min) {
      overall_min = min_value;
    }
  }
  size_t gap = overall_max - overall_min;
  double mean = (double)average / volume;
  double relative_gap = (double)gap / mean;

  printf("relative gap: %f, ", relative_gap);

  return relative_gap < 0.6;
}

template <class function_type>
bool any_possible_pair_at_the_start(const function_type &function) {
  constexpr size_t size = 64;
  uint64_t input[size];
  std::bitset<size * size> bits;
  for (size_t trial = 0; trial < size * size * size; trial++) {
    // We always start from the same input.
    std::iota(input, input + size, 0);
    // We shuffle:
    function(input, size);
    bits[input[0] * size + input[1]] = 1;
  }
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (i == j) {
        if (bits[i * size + j]) {
          return false;
        }
      } else {
        if (!bits[i * size + j]) {
          return false;
        }
      }
    }
  }
  return true;
}

template <class function_type>
bool any_possible_pair_at_the_end(const function_type &function) {
  constexpr size_t size = 64;
  uint64_t input[size];
  std::bitset<size * size> bits;
  for (size_t trial = 0; trial < size * size * size; trial++) {
    // We always start from the same input.
    std::iota(input, input + size, 0);
    // We shuffle:
    function(input, size);
    bits[input[0] * size + input[1]] = 1;
  }
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (i == j) {
        if (bits[i * size + j]) {
          return false;
        }
      } else {
        if (!bits[i * size + j]) {
          return false;
        }
      }
    }
  }
  return true;
}

struct named_function {
  std::string name;
  shuffle_function function;
};

named_function func[] = {
    {"shuffle_lehmer", shuffle_lehmer},
    {"shuffle_lehmer_2", shuffle_lehmer_2},
    {"shuffle_lehmer_23456", shuffle_lehmer_23456},
    {"shuffle_pcg", shuffle_pcg},
    {"shuffle_pcg_2", shuffle_pcg_2},
    {"shuffle_pcg_23456", shuffle_pcg_23456}
};

bool test_everyone_can_move_everywhere() {
  std::cout << __FUNCTION__ << std::endl;
  for (const auto &f : func) {
    std::cout << std::setw(40) << f.name << ": ";
    std::cout.flush();
    if (!everyone_can_move_everywhere(f.function)) {
      std::cerr << "!!!Test failed for " << f.name << std::endl;
      return false;
    } else {
      std::cout << "passed" << std::endl;
    }
  }
  return true;
}

bool test_uniformity_test() {
  std::cout << __FUNCTION__ << std::endl;
  for (const auto &f : func) {
    std::cout << std::setw(40) << f.name << ": ";
    std::cout.flush();
    if (!uniformity_test(f.function)) {
      std::cerr << "!!!Test failed for " << f.name << std::endl;
      return false;
    } else {
      std::cout << "passed" << std::endl;
    }
  }
  return true;
}

bool test_any_possible_pair_at_the_start() {
  std::cout << __FUNCTION__ << std::endl;
  for (const auto &f : func) {
    std::cout << std::setw(40) << f.name << ": ";
    std::cout.flush();
    if (!any_possible_pair_at_the_start(f.function)) {
      std::cerr << "!!!Test failed for " << f.name << std::endl;
      return false;
    } else {
      std::cout << "passed" << std::endl;
    }
  }
  return true;
}

bool test_any_possible_pair_at_the_end() {
  std::cout << __FUNCTION__ << std::endl;
  for (const auto &f : func) {
    std::cout << std::setw(40) << f.name << ": ";
    std::cout.flush();
    if (!any_possible_pair_at_the_end(f.function)) {
      std::cerr << "!!!Test failed for " << f.name << std::endl;
      return false;
    } else {
      std::cout << "passed" << std::endl;
    }
  }
  return true;
}

int main() {
  seed(1234);
  bool success = true;
  success &= test_uniformity_test();
  success &= test_any_possible_pair_at_the_end();
  success &= test_any_possible_pair_at_the_start();
  success &= test_everyone_can_move_everywhere();
  if (success) {
    std::cout << "All tests passed" << std::endl;
  } else {
    std::cerr << "Some tests failed" << std::endl;
  }
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
