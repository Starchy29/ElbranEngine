#pragma once
struct Vector2;
struct Matrix;
struct Circle;
struct Rectangle;

#define PI 3.1415927f

struct Vector2 {
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);

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

	Circle(Vector2 center, float radius);

	bool Contains(const Vector2& point) const;
	bool Contains(const Circle& other) const;
	bool Intersects(const Circle& other) const;
	bool Intersects(const Rectangle& rectangle) const;
};

struct Rectangle {
	float left;
	float right;
	float top;
	float bottom;

	Rectangle(Vector2 center, Vector2 size);
	Rectangle(float left, float right, float top, float bottom);

	Vector2 Center() const;
	Vector2 Size() const;

	Rectangle SetCenter(Vector2 center) const;
	Rectangle SetWidth(float width) const;
	Rectangle SetHeight(float height) const;
	Rectangle SetSize(Vector2 size) const;
	Rectangle Expand(float shiftPerSide) const;

	bool Contains(const Vector2& point) const;
	bool Contains(const Rectangle& other) const;
	bool Intersects(const Rectangle& other) const;
	bool Intersects(const Circle& circle) const;
};

struct Matrix {
	float values[4][4];

	Matrix operator-() const;
	
	static Matrix Rotation(float radians);
	static Matrix Scale(float x, float y);
	static Matrix Translate(float x, float y);

	static const Matrix Identity;
};

Vector2 operator*(const Matrix& transform, const Vector2& vector);
Matrix operator*(const float scalar, const Matrix& matrix);
Matrix operator+(const Matrix& left, const Matrix& right);
Matrix operator-(const Matrix& left, const Matrix& right);
Matrix operator*(const Matrix& left, const Matrix& right);