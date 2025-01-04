#include "Random.h"
#include <cctype>
#include <random>

std::mt19937* generator;

int Random::Generate(int min, int max) const {
	std::uniform_int_distribution<uint_least32_t> distribute = std::uniform_int_distribution<uint_least32_t>(min, max);
	return distribute(*generator);
}

float Random::Generate(float min, float max) const {
	std::uniform_real_distribution<float> distribute = std::uniform_real_distribution<float>(min, max);
	return distribute(*generator);
}

Random::Random() {
	std::random_device os_seed;
	uint_least32_t seed = os_seed();
	generator = new std::mt19937(seed);
}

Random::~Random() { 
	delete generator;
}
