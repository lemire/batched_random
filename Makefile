all:    benchmark 
benchmark: benchmarks/benchmark.cpp random_bounded.o
	c++ $(CXXFLAGS) -std=c++17 -O3 -Wall -o benchmark benchmarks/benchmark.cpp random_bounded.o  -Iinclude -Ibenchmarks 

random_bounded.o: src/batch_shuffle_dice.c src/random_bounded.c include/random_bounded.h src/lehmer64.h  src/splitmix64.h
	cc $(CFLAGS) -std=c11 -O3 -Wall -Wextra -Wconversion -c src/random_bounded.c


clean:
	rm -f random_bounded.o benchmark