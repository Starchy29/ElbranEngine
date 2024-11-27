#pragma once
#include "Vector2.h"

struct Circle
{
	Vector2 center;
	float radius;

	Circle(Vector2 center, float radius) {
		this->center = center;
		this->radius = radius;
	}

	bool Contains(const Vector2 &point) const {
		return center.SqrDist(point) <= radius * radius;
	}

	bool Contains(const Circle &other) const {
		return center.Distance(other.center) + other.radius <= radius;
	}

	bool Intersects(const Circle &other) const {
		float radSum = radius + other.radius;
		return center.SqrDist(other.center) <= radSum * radSum;
	}
};

