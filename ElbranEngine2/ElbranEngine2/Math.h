#pragma once
struct Vector2;
struct Matrix;
struct Circle;
struct AlignedRect;

#define PI 3.1415927f
#define SQRT2_DIV2 0.71f

namespace Math {
	float Sign(float number);
	int Sign(int number);
	float Clamp(float value, float minimum, float maximum);
	int Clamp(int value, int minimum, int maximum);
	float FractionOf(float number);
	float InvSqrt(float number);
}

namespace Tween {
	float Lerp(float start, float end, float t);
	Vector2 Lerp(Vector2 start, Vector2 end, float t);

	// domain and range of these are [0, 1]
	float SlowToFast(float t, float strength = 2.0f); // strength is 1-inf
	float FastToSlow(float t, float strength = 2.0f); // strength is 1-inf
	float EaseInOut(float t, float minSpeed = 0.f); // slow to fast to slow, minSpeed is 0-1
	float SpeedInOut(float t, float minSpeed = 0.f); // fast to slow to fast, minSpeed is 0-1
	float Rebound(float t); // upside down v
	float Bounce(float t, float strength = 2.0f); // rebound that accelerates towards the direction change, strength is 1-inf
	float Arc(float t); // inverse parabola
}

struct Vector2 {
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);
	static Vector2 FromAngle(float radians);

	float Length() const;
	float LengthSquared() const;
	float Angle() const;

	float Dot(const Vector2& other) const;
	float Distance(const Vector2& other) const;
	float SquareDistance(const Vector2& other) const;
	float AngleBetween(const Vector2& other) const;

	Vector2 Normalize() const;
	Vector2 Rotate(float radians) const;
	Vector2 SetLength(float length) const;
	Vector2 GetPerpendicular(bool rightSide = true) const;
	Vector2 ProjectOnto(const Vector2& other) const;

	Vector2 operator-() const;
	Vector2& operator+= (const Vector2& vec);
	Vector2& operator-= (const Vector2& vec);
	Vector2& operator*= (const Vector2& vec);
	Vector2& operator/= (const Vector2& vec);
	Vector2& operator*= (float scalar);
	Vector2& operator/= (float scalar);

	static const Vector2 Zero;
	static const Vector2 Up;
	static const Vector2 Down;
	static const Vector2 Left;
	static const Vector2 Right;
};

bool operator==(const Vector2& left, const Vector2& right);
bool operator!=(const Vector2& left, const Vector2& right);
Vector2 operator+(const Vector2& left, const Vector2& right);
Vector2 operator-(const Vector2& left, const Vector2& right);
Vector2 operator*(const Vector2& left, const Vector2& right);
Vector2 operator/(const Vector2& left, const Vector2& right);

Vector2 operator*(const Vector2& vector, const float& scalar);
Vector2 operator*(const float& scalar, const Vector2& vector);
Vector2 operator/(const Vector2& vector, const float& scalar);


struct Circle {
	Vector2 center;
	float radius;

	Circle();
	Circle(Vector2 center, float radius);

	bool Contains(const Vector2& point) const;
	bool Contains(const Circle& other) const;
	bool Intersects(const Circle& other) const;
	bool Intersects(const AlignedRect& rectangle) const;
};

struct AlignedRect {
	float left;
	float right;
	float top;
	float bottom;

	AlignedRect();
	AlignedRect(Vector2 center, Vector2 size);
	AlignedRect(float left, float right, float top, float bottom);

	Vector2 Center() const;
	Vector2 Size() const;
	float Width() const;
	float Height() const;

	AlignedRect SetCenter(Vector2 center) const;
	AlignedRect Translate(Vector2 shift) const;
	AlignedRect SetWidth(float width) const;
	AlignedRect SetHeight(float height) const;
	AlignedRect SetSize(Vector2 size) const;
	AlignedRect Expand(float shiftPerSide) const;

	bool Contains(const Vector2& point) const;
	bool Contains(const AlignedRect& other) const;
	bool Intersects(const AlignedRect& other) const;
	bool Intersects(const Circle& circle) const;
};

struct Matrix {
	float values[4][4];

	float Determinant() const;
	Matrix Transpose() const;
	Matrix Inverse() const;

	static const Matrix Identity;
	
	static Matrix Rotation(float radians);
	static Matrix Scale(float x, float y);
	static Matrix Translation(float x, float y, float z = 0.f);
	static Matrix View(Vector2 eyePosition, float rotation);
	static Matrix ProjectOrthographic(float viewWidth, float viewHeight, float viewRange);

	Matrix operator-() const;

private:
	void CreateSubmatrix(float outSubmatrix[3][3], int removedRow, int removedCol) const;
	static float Calc3x3Determinant(float matrix[3][3]);
};

Vector2 operator*(const Matrix& transform, const Vector2& vector);
Matrix operator*(const float scalar, const Matrix& matrix);
Matrix operator+(const Matrix& left, const Matrix& right);
Matrix operator-(const Matrix& left, const Matrix& right);
Matrix operator*(const Matrix& left, const Matrix& right);