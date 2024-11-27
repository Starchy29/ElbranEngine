#pragma once
#include <SimpleMath.h>

struct Circle
{
	DirectX::SimpleMath::Vector2 center;
	float radius;

	Circle(DirectX::SimpleMath::Vector2 center, float radius) {
		this->center = center;
		this->radius = radius;
	}

	bool Contains(const DirectX::SimpleMath::Vector2 &point) const {
		return DirectX::SimpleMath::Vector2::DistanceSquared(center, point) <= radius * radius;
	}

	bool Contains(const Circle &other) const {
		return DirectX::SimpleMath::Vector2::Distance(center, other.center) + other.radius <= radius;
	}

	bool Intersects(const Circle &other) const {
		float radSum = radius + other.radius;
		return DirectX::SimpleMath::Vector2::DistanceSquared(center, other.center) <= radSum * radSum;
	}
};

