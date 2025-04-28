#pragma once
#include "Math.h"

#define PERLIN_OCTAVES 6

class Random {
public:
	Random();
	~Random();

	int GenerateInt(int min, int max) const;
	float GenerateFloat(float min, float max) const;
	Vector2 GenerateInCircle() const;
	Vector2 GenerateOnCircle() const;
	float Perlin(float x, float y = 0.0f) const; // range: [-1, 1]

private:
	static constexpr int primes[PERLIN_OCTAVES + 2] = {
		995615039, 831731269, 174329291, 362489573, 457025711, 787070341, 405493717, 458904767
	};

	float CosLerp(float start, float end, float t) const;
	float GetNoise(int x, int y, int octave) const; // range: [-1, 1]
	float GetSmoothNoise(int x, int y, int octave) const;
	float GetInterpolatedNoise(float x, float y, int octave) const;
};