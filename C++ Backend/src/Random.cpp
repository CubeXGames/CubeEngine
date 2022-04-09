#include "Random.h"
#include <chrono>

uint64_t Random::seedUniquifier = 0x27BA309F5B417407;
Random Random::random = Random();

Random::Random() {

	state = std::chrono::system_clock::now().time_since_epoch().count() ^ Random::uniquifySeed();
}

Random::Random(uint64_t seed) {

	state = seed;
}

void Random::setSeed(uint64_t seed) { state = seed; }

int Random::nextBit() {

	uint64_t bit = ((state >> 0) ^ (state >> 2) ^ (state >> 3) ^ (state >> 5)) & 1ULL;
	state = (state >> 1) | (bit << 63);
	return (int)bit;
}

/*int Random::nextBits(int numBits) {

	if (numBits <= 0) return 0;
	if (numBits > sizeof(int)) return 0;

	int number = 0;
	for (int i = 0; i < numBits; i++) {

		number = number << 1;
		number += nextBit();
	}

	return number;
}*/

/*int Random::nextInt(int bound) {

	if (bound <= 0) return 0;
	int r = nextBits(31);
	int m = bound - 1;
	if ((bound & m) == 0) r = (int)((bound * (long)r) >> 31);
	else {

		for (int u = r;
			u - (r = u % bound) + m < 0;
			u = nextBits(31));
	}

	return r;
}*/

uint64_t Random::uniquifySeed() {

	uint64_t next = seedUniquifier * 1181783497276652981ULL;
	seedUniquifier = next;
	return seedUniquifier;
}