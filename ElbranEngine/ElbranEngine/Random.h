#pragma once
#include "Vector2.h"

#define PERLIN_OCTAVES 6

class Random
{
	friend class Application;

public:
	int GenerateInt(int min, int max) const;
	float GenerateFloat(float min, float max) const;
	Vector2 GenerateInCircle(float radius = 1.0f) const;
	Vector2 GenerateOnCircle() const;
	float Perlin(float x, float y = 0.0f) const; // range: [-1, 1]

	// prevent copying
	Random(const Random&) = delete;
	void operator=(const Random&) = delete;

private:
	static constexpr int primes[PERLIN_OCTAVES + 2] = {
		995615039, 831731269, 174329291, 362489573, 457025711, 787070341, 405493717, 458904767
	};

	Random();
	~Random();

	float CosLerp(float start, float end, float t) const;
	float GetNoise(int x, int y, int octave) const; // range: [-1, 1]
	float GetSmoothNoise(int x, int y, int octave) const;
	float GetInterpolatedNoise(float x, float y, int octave) const;
};

