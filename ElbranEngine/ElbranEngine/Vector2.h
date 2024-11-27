#pragma once
#include <DirectXMath.h>
#include <string>

struct Vector2
{
	static const Vector2 Zero;
	static const Vector2 Up;
	static const Vector2 Down;
	static const Vector2 Left;
	static const Vector2 Right;

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
	Vector2 Normalize() const;
	std::string ToString() const;

	Vector2 operator-() const;
	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	Vector2 operator*(const Vector2& other) const;
	operator DirectX::XMFLOAT2() const;
};

