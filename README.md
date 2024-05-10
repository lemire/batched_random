## Batched Random

We benchmark fast shuffling functions using batched random index generation.

### Requirements

- Recent LLVM clang and clang++ compilers
- Make


### Running Benchmarks


```
make
./benchmark
```

To run tests:
```
./basic
```

## Code

See `src` directory for the main code.

## Other Compilers

The code is portable and other compilers can be used. The performance
of the C++ code might be sensitive to the C++ compiler used.
