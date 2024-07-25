all:    benchmark basic stream
CXX=c++
CC=cc
benchmark: benchmarks/benchmark.cpp random_bounded.o
	$(CXX) $(CXXFLAGS) -std=c++17 -O3 -Wall  -Wextra  -o benchmark benchmarks/benchmark.cpp random_bounded.o  -Iinclude -Ibenchmarks 
stream: benchmarks/stream.cpp random_bounded.o
	$(CXX) $(CXXFLAGS) -std=c++17 -O3 -Wall  -Wextra  -o stream benchmarks/stream.cpp random_bounded.o  -Iinclude -Ibenchmarks 
basic : tests/basic.cpp random_bounded.o
	$(CXX) $(CXXFLAGS) -std=c++17 -O3 -Wall  -Wextra  -o basic tests/basic.cpp random_bounded.o  -Iinclude
random_bounded.o: src/batch_shuffle_dice.c src/random_bounded.c include/random_bounded.h src/lehmer64.h  src/splitmix64.h
	$(CC) $(CFLAGS) -std=c11 -O3 -Wall -Wextra -Wconversion -c src/random_bounded.c

clean:
	rm -f random_bounded.o benchmark basic stream
