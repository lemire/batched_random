## Batched Random

We benchmark fast shuffling functions using batched random index generation.
It is meant for research purposes. Though we have good benchmarks and tests, 
this code is not meant to be production-ready.

### Requirements

- Recent LLVM clang and clang++ compilers
- Make

### Running Benchmarks


```
make
./benchmark
```

To get the C++ benchmarks, you can type `./benchmark --cpp`.

To run tests:
```
./basic
```

## Code

See `src` directory for the main code.

## Other Compilers

We use LLVM/clang for benchmarking. The code is portable and other compilers can be used. The performance is
sensitive to the compiler used.

GCC users may get better results with `shuffle 2` as opposed to `shuffle 2-6`. The reverse is true for LLVM
users.