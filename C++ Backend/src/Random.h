#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

class Random {

public:

	static uint64_t seedUniquifier;
	static Random random;

	Random();
	Random(uint64_t seed);
	void setSeed(uint64_t seed);
	int nextBit();
	//int nextBits(int bits);
	//int nextInt(int bound);
	static uint64_t uniquifySeed();

private:

	uint64_t state;
};

#endif