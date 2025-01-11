#include "Random.h"
#include <cctype>
#include <random>

std::mt19937* generator;

int Random::GenerateInt(int min, int max) const {
	std::uniform_int_distribution<uint_least32_t> distribute = std::uniform_int_distribution<uint_least32_t>(min, max);
	return distribute(*generator);
}

float Random::GenerateFloat(float min, float max) const {
	std::uniform_real_distribution<float> distribute = std::uniform_real_distribution<float>(min, max);
	return distribute(*generator);
}

Vector2 Random::GenerateInCircle(float radius) const {
	float angle = GenerateFloat(0.f, DirectX::XM_2PI);
	float length = radius * sqrt(GenerateFloat(0.f, 1.0f));
	return length * Vector2(cos(angle), sin(angle));
}

Vector2 Random::GenerateOnCircle() const {
	float angle = GenerateFloat(0.f, DirectX::XM_2PI);
	return Vector2(cos(angle), sin(angle));
}

Random::Random() {
	std::random_device os_seed;
	uint_least32_t seed = os_seed();
	generator = new std::mt19937(seed);
}

Random::~Random() { 
	delete generator;
}
