#pragma once
#include "Vector2.h"

class Random
{
	friend class Application;

public:
	int GenerateInt(int min, int max) const;
	float GenerateFloat(float min, float max) const;
	Vector2 GenerateInCircle(float radius = 1.0f) const;
	Vector2 GenerateOnCircle() const;

	// prevent copying
	Random(const Random&) = delete;
	void operator=(const Random&) = delete;

private:
	Random();
	~Random();
};

