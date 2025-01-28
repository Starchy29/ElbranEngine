#include "Random.h"
#include <cctype>
#include <random>

#define PERLIN_PERSISTENCE 0.25f

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

float Random::Perlin(float x, float y) const {
	float total = 0.f;

	for(int octave = 0; octave < PERLIN_OCTAVES; octave++) {
		float frequency = pow(2.f, octave);
		float amplitude = pow(PERLIN_PERSISTENCE, octave);

		total += amplitude * GetInterpolatedNoise(x * frequency, y * frequency, octave);
	}

	return total;
}

Random::Random() {
	std::random_device os_seed;
	uint_least32_t seed = os_seed();
	generator = new std::mt19937(seed);
}

Random::~Random() { 
	delete generator;
}

float Random::CosLerp(float start, float end, float t) const {
	t *= DirectX::XM_PI;
	t = (1.0f - cos(t)) * 0.5f;
	return start * (1.0f - t) + end * t;
}

float Random::GetNoise(int x, int y, int octave) const {
	int n = x + 57 * y;
	n = (n << 13) ^ n;
	return 1.0f - ((n * (n * n * primes[octave] + primes[octave + 1]) + primes[octave + 2]) & 0x7fffffff) / 1073741824.0f;
}

float Random::GetSmoothNoise(int x, int y, int octave) const {
	float center = GetNoise(x, y, octave) * 0.25f; // 1/4
	float sides = (GetNoise(x - 1, y, octave) + GetNoise(x + 1, y, octave) + GetNoise(x, y + 1, octave) + GetNoise(x, y - 1, octave)) * 0.125f; // 1/8
	float corners = (GetNoise(x - 1, y - 1, octave) + GetNoise(x + 1, y - 1, octave) + GetNoise(x - 1, y + 1, octave) + GetNoise(x + 1, y + 1, octave)) * 0.0625; // 1/16
	
	// 4 * 1/16 + 4 * 1/8 + 1/4 = 1
	return center + sides + corners;
}

float Random::GetInterpolatedNoise(float x, float y, int octave) const {
	int xInt = (int)x;
	int yInt = (int)y;
	float xFraction = x - floor(x);
	float yFraction = y - floor(y);

	// interpolate the top and bottom sides across, then interpolate vertically between them
	return CosLerp(
		CosLerp(GetSmoothNoise(xInt, yInt, octave), // bottom left
			GetSmoothNoise(xInt + 1, yInt, octave), // bottom right
			xFraction
		),
		CosLerp(GetSmoothNoise(xInt, yInt + 1, octave), // top left
			GetSmoothNoise(xInt + 1, yInt + 1, octave), // top right
			xFraction
		),
		yFraction
	);
}