#pragma once
#include <DirectXMath.h>

#define ZERO_VECTOR Vector2(0, 0)
#define RIGHT_VECTOR Vector2(1, 0)
#define LEFT_VECTOR Vector2(-1, 0)
#define UP_VECTOR Vector2(0, 1)
#define DOWN_VECTOR Vector2(0, -1)

struct Vector2
{
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);
	Vector2(DirectX::XMFLOAT2 float2);

	void Rotate(float radians);

	float Length() const;
	float Angle() const;
	float Distance(const Vector2 &other) const;
	float SqrDist(const Vector2& other) const;
	float Dot(const Vector2 &other) const;
	Vector2 GetPerpendicular(bool rightSide = true) const;

	Vector2 operator-() const;
	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	Vector2 operator*(const Vector2& other) const;
	operator DirectX::XMFLOAT2() const;
};

