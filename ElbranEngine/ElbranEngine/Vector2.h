#pragma once
#include <DirectXMath.h>
#include <string>

struct Vector2 : public DirectX::XMFLOAT2
{
	static const Vector2 Zero;
	static const Vector2 Up;
	static const Vector2 Down;
	static const Vector2 Left;
	static const Vector2 Right;

	Vector2() : XMFLOAT2(0, 0) {}
	Vector2(float x, float y) : XMFLOAT2(x, y) {}
	Vector2(const XMFLOAT2& v) {
		x = v.x;
		y = v.y;
	}

	void Rotate(float radians);

	float Length() const;
	float Angle() const;
	float Distance(const Vector2 &other) const;
	float SqrDist(const Vector2& other) const;
	float Dot(const Vector2 &other) const;
	Vector2 GetPerpendicular(bool rightSide = true) const;
	Vector2 Normalize() const;
	std::string ToString() const;

	Vector2 operator-() const;
	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	Vector2 operator*(const Vector2& other) const;
};

