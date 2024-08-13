## Batched Random

We benchmark fast shuffling functions using batched random index generation.
It is meant for research purposes. Though we have good benchmarks and tests, 
this code is not meant to be production-ready.

### Reference

* Nevin Brackett-Rozinsky, Daniel Lemire, [Batched Ranged Random Integer Generation](https://arxiv.org/abs/2408.06213), Software: Practice and Experience (to appear)

### Requirements

- Recent LLVM clang and clang++ compilers
- Make

### Running Benchmarks


```
make
./benchmark
```

To get the C++ benchmarks, you can type `./benchmark --cpp`. They are disabled by default.

To run tests:
```
./basic
```

## Code

See `src` directory for the main code.

## Other Compilers

We use LLVM/clang for benchmarking.

The code is portable and other compilers can be used. The performance
of the C++ code might be sensitive to the C++ compiler used.
Specifically, we find that `shuffle_23456` has relatively poor performance
with GCC compared to LLVM/clang. GCC fails to optimize `shuffle_23456` properly.
We recommend that GCC users prefer `shuffle_2`.
