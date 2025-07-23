#include "Math.h"
#include <math.h>

#pragma region Math
float Math::Sign(float number) {
	if(number > 0.f) return 1.0f;
	if(number < 0.f) return -1.0f;
	return 0.0f;
}

int Math::Sign(int number) {
	if(number > 0) return 1;
	if(number < 0) return -1;
	return 0;
}

float Math::Clamp(float value, float minimum, float maximum) {
	if(value > maximum) return maximum;
	return (value < minimum ? minimum : value);
}

int Math::Clamp(int value, int minimum, int maximum) {
	if(value > maximum) return maximum;
	return (value < minimum ? minimum : value);
}

float Math::FractionOf(float number) {
	float integral;
	float remainder = modff(number, &integral);
	return (number > 0.f ? remainder : -remainder); // modff returns a negative if the input is negative
}

float Math::InvSqrt(float number) {
	// fast inverse square root algorithm, based on the fact that an IEEE 754 float's bits are an approximation of its own logarithm
	float halfInput = number * 0.5f;
	long log = * (long*) &number; // reinterpret as a 32-bit integer to allow bit manipulation
	log = 0x5f3759df - (log >> 1); // inverse square root is -1/2 power, constant is the error term
	number = * (float*) &log; // reinterpret back as a float
	number = number * (1.5f - halfInput * number * number); // Newton iteration improves approximation
	number = number * (1.5f - halfInput * number * number);
	return number;
}
#pragma endregion

#pragma region Tween
float Tween::Lerp(float start, float end, float t) {
	return t * end + (1.0f - t) * start;
}

Vector2 Tween::Lerp(Vector2 start, Vector2 end, float t) {
	return t * end + (1.0f - t) * start;
}

float Tween::SlowToFast(float t, float strength) {
	return powf(t, strength);
}

float Tween::FastToSlow(float t, float strength) {
	return powf(t, 1.0f / strength);
}

float Tween::EaseInOut(float t, float minSpeed) {
	// piecewise function whose derivative is half a quadratic parabola on either side of x=0.5, contrained for (0.5,0.5) being on the curve
	Vector2 shift = t < 0.5f ? Vector2::Zero : Vector2(1.f, 1.f);
	float base = t - shift.x;
	return (4.f - 4.f * minSpeed) * base * base * base + minSpeed * base + shift.y;
}

float Tween::SpeedInOut(float t, float minSpeed) {
	// the derivative of this function is a quadratic with a point at (0.5, minSpeed)
	return (4.f - 4.f * minSpeed) * t * t * t + (-6.f + 6.f * minSpeed) * t * t + (3.f - 2.f * minSpeed) * t;
}

float Tween::Rebound(float t) {
	return 1.0f - 2.0f * fabsf(t - 0.5f);
}

float Tween::Bounce(float t, float strength) {
	return powf(Rebound(t), strength);
}

float Tween::Arc(float t) {
	return -4 * t * t + 4 * t;
}
#pragma endregion

#pragma region Vector2
const Vector2 Vector2::Zero = Vector2(0.f, 0.f);
const Vector2 Vector2::Up = Vector2(0.f, 1.f);
const Vector2 Vector2::Down = Vector2(0.f, -1.f);
const Vector2 Vector2::Left = Vector2(-1.f, 0.f);
const Vector2 Vector2::Right = Vector2(1.f, 0.f);

Vector2::Vector2() {
	x = 0.f;
	y = 0.f;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}

Vector2 Vector2::FromAngle(float radians) {
	return Vector2(cosf(radians), sinf(radians));
}

float Vector2::Length() const {
	return sqrtf(x * x + y * y);
}

float Vector2::LengthSquared() const {
	return x * x + y * y;
}

float Vector2::Angle() const {
	return atan2f(y, x);
}

float Vector2::Dot(const Vector2& other) const {
	return x * other.x + y * other.y;
}

float Vector2::Distance(const Vector2& other) const {
	float xDiff = x - other.x;
	float yDiff = y - other.y;
	return sqrtf(xDiff * xDiff + yDiff * yDiff);
}

float Vector2::SquareDistance(const Vector2& other) const {
	float xDiff = x - other.x;
	float yDiff = y - other.y;
	return xDiff * xDiff + yDiff * yDiff;
}
float Vector2::AngleBetween(const Vector2& other) const {
	return acosf(Dot(other) / (Length() * other.Length()));
}

Vector2 Vector2::Normalize() const {
	if(x == 0.f && y == 0.f) return Vector2::Zero;
	if(x == 0.f || y == 0.f) return Vector2(Math::Sign(x), Math::Sign(y));
	return *this * Math::InvSqrt(x * x + y * y);
}

Vector2 Vector2::Rotate(float radians) const {
	// shortcut multiplication with a 2D rotation matrix
	float cos = cosf(radians);
	float sin = sinf(radians);
	return Vector2(cos * x - sin * y, sin * x + cos * y);
}

Vector2 Vector2::SetLength(float length) const {
	return Normalize() * length;
}

Vector2 Vector2::GetPerpendicular(bool rightSide) const {
	Vector2 result = Vector2(-y, x);
	if(rightSide) {
		result = -result;
	}
	return result;
}

Vector2 Vector2::ProjectOnto(const Vector2& other) const {
	Vector2 norm = other.Normalize();
	return Dot(norm) * norm;
}

Vector2 Vector2::operator-() const {
	return Vector2(-x, -y);
}

bool operator==(const Vector2& left, const Vector2& right) {
	return left.x == right.x && left.y == right.y;
}

bool operator!=(const Vector2& left, const Vector2& right) {
	return left.x != right.x || left.y != right.y;;
}

Vector2 operator+(const Vector2& left, const Vector2& right) {
	return Vector2(left.x + right.x, left.y + right.y);
}

Vector2 operator-(const Vector2& left, const Vector2& right) {
	return Vector2(left.x - right.x, left.y - right.y);
}

Vector2 operator*(const Vector2& left, const Vector2& right) {
	return Vector2(left.x * right.x, left.y * right.y);
}

Vector2 operator/(const Vector2& left, const Vector2& right) {
	return Vector2(left.x / right.x, left.y / right.y);
}

Vector2 operator*(const Vector2& vector, const float& scalar) {
	return Vector2(scalar * vector.x, scalar * vector.y);
}

Vector2 operator*(const float& scalar, const Vector2& vector) {
	return Vector2(scalar * vector.x, scalar * vector.y);
}

Vector2 operator/(const Vector2& vector, const float& scalar) {
	return Vector2(vector.x / scalar, vector.y / scalar);
}

Vector2& Vector2::operator+=(const Vector2& vec) {
	*this = *this + vec;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& vec) {
	*this = *this - vec;
	return *this;
}

Vector2& Vector2::operator*=(const Vector2& vec) {
	*this = *this * vec;
	return *this;
}

Vector2& Vector2::operator/=(const Vector2& vec) {
	*this = *this / vec;
	return *this;
}

Vector2& Vector2::operator*=(float scalar) {
	*this = *this * scalar;
	return *this;
}

Vector2& Vector2::operator/=(float scalar) {
	*this = *this / scalar;
	return *this;
}
#pragma endregion

#pragma region Circle
Circle::Circle() {
	center = {};
	radius = 0.f;
}

Circle::Circle(Vector2 center, float radius) {
	this->center = center;
	this->radius = radius;
}

bool Circle::Contains(const Vector2& point) const {
	return point.SquareDistance(center) <= radius * radius;
}

bool Circle::Contains(const Circle& other) const {
	float radDiff = radius - other.radius;
	return center.SquareDistance(other.center) <= radDiff * radDiff;
}

bool Circle::Intersects(const Circle& other) const {
	float radSum = radius + other.radius;
	return center.SquareDistance(other.center) <= radSum * radSum;
}

bool Circle::Intersects(const AlignedRect& rectangle) const {
	return rectangle.Intersects(*this);
}
#pragma endregion

#pragma region AlignedRect
AlignedRect::AlignedRect() {
	left = 0.f;
	right = 0.f;
	top = 0.f;
	bottom = 0.f;
}

AlignedRect::AlignedRect(Vector2 center, Vector2 size) {
	left = center.x - size.x / 2.0f;
	right = center.x + size.x / 2.0f;
	top = center.y + size.y / 2.0f;
	bottom = center.y - size.y / 2.0f;
}

AlignedRect::AlignedRect(float left, float right, float top, float bottom) {
	this->left = left;
	this->right = right;
	this->top = top;
	this->bottom = bottom;
}

Vector2 AlignedRect::Center() const {
	return Vector2((right + left) / 2.0f, (top + bottom) / 2.0f);
}

Vector2 AlignedRect::Size() const {
	return Vector2(right - left, top - bottom);
}

float AlignedRect::Width() const {
	return right - left;
}

float AlignedRect::Height() const {
	return top - bottom;
}

AlignedRect AlignedRect::SetCenter(Vector2 center) const {
	return AlignedRect(center, Size());
}

AlignedRect AlignedRect::Translate(Vector2 shift) const {
	return AlignedRect(Center() + shift, Size());
}

AlignedRect AlignedRect::SetWidth(float width) const {
	return AlignedRect(Center(), Vector2(width, top - bottom));
}

AlignedRect AlignedRect::SetHeight(float height) const {
	return AlignedRect(Center(), Vector2(right - left, height));
}

AlignedRect AlignedRect::SetSize(Vector2 size) const {
	return AlignedRect(Center(), size);
}

AlignedRect AlignedRect::Expand(float shiftPerSide) const {
	return AlignedRect(left - shiftPerSide, right + shiftPerSide, top + shiftPerSide, bottom - shiftPerSide);
}

bool AlignedRect::Contains(const Vector2& point) const {
	return point.x >= left && point.x <= right && point.y <= top && point.y >= bottom;
}

bool AlignedRect::Contains(const AlignedRect& other) const {
	return other.left >= left && other.right <= right && other.top <= top && other.bottom >= bottom;
}

bool AlignedRect::Intersects(const AlignedRect& other) const {
	return !(right < other.left || left > other.right || top < other.bottom || bottom > other.top);
}

bool AlignedRect::Intersects(const Circle& circle) const {
	if(circle.center.x >= left && circle.center.x <= right) {
		return fabsf(circle.center.y - Center().y) < circle.radius + (top - bottom) / 2.0f;
	}

	if(circle.center.y >= bottom && circle.center.y <= top) {
		return fabsf(circle.center.x - Center().x) < circle.radius + (right - left) / 2.0f;
	}

	Vector2 corner = Vector2(circle.center.x < left ? left : right, circle.center.y < bottom ? bottom : top);
	return circle.center.SquareDistance(corner) <= circle.radius * circle.radius;
}
#pragma endregion

#pragma region Matrix
Matrix operator+(const Matrix& left, const Matrix& right) {
	Matrix result;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			result.values[r][c] = left.values[r][c] + right.values[r][c];
		}
	}
	return result;
}

Matrix operator-(const Matrix& left, const Matrix& right) {
	Matrix result;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			result.values[r][c] = left.values[r][c] - right.values[r][c];
		}
	}
	return result;
}

Vector2 operator*(const Matrix& transform, const Vector2& vector) {
	// assume the vector is (x, y, 0, 1)
	return Vector2(
		transform.values[0][0] * vector.x + transform.values[0][1] * vector.y + transform.values[0][3],
		transform.values[1][0] * vector.x + transform.values[1][1] * vector.y + transform.values[1][3]
	);
}

Matrix operator*(const float scalar, const Matrix& matrix) {
	Matrix result;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			result.values[r][c] = scalar * matrix.values[r][c];
		}
	}

	return result;
}

Matrix operator*(const Matrix& left, const Matrix& right) {
	Matrix result;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			result.values[r][c] = 0.f;
			for(int i = 0; i < 4; i++) {
				result.values[r][c] += left.values[r][i] * right.values[i][c];
			}
		}
	}

	return result;
}

const Matrix Matrix::Identity {{
	{ 1.f, 0.f, 0.f, 0.f },
	{ 0.f, 1.f, 0.f, 0.f },
	{ 0.f, 0.f, 1.f, 0.f },
	{ 0.f, 0.f, 0.f, 1.f }
}};

Matrix Matrix::operator-() const {
	Matrix result;
	for(int r = 0; r < 4; r++) {
		for(int c = 0; c < 4; c++) {
			result.values[r][c] *= -1.f;
		}
	}
	return result;
}

Matrix Matrix::Transpose() const {
	return Matrix{ {
		{ values[0][0], values[1][0], values[2][0], values[3][0]},
		{ values[0][1], values[1][1], values[2][1], values[3][1] },
		{ values[0][2], values[1][2], values[2][2], values[3][2] },
		{ values[0][3], values[1][3], values[2][3], values[3][3] }
	} };
}

Matrix Matrix::Rotation(float radians) {
	return Matrix{ {
		{ cosf(radians), -sinf(radians), 0.f, 0.f },
		{ sinf(radians), cosf(radians), 0.f, 0.f },
		{ 0.f, 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 0.f, 1.f }
	} };
}

Matrix Matrix::Scale(float x, float y) {
	return Matrix{ {
		{   x, 0.f, 0.f, 0.f },
		{ 0.f,   y, 0.f, 0.f },
		{ 0.f, 0.f, 1.f, 0.f },
		{ 0.f, 0.f, 0.f, 1.f }
	} };
}

Matrix Matrix::Translation(float x, float y, float z) {
	return Matrix{ {
		{ 1.f, 0.f, 0.f, x },
		{ 0.f, 1.f, 0.f, y },
		{ 0.f, 0.f, 1.f, z },
		{ 0.f, 0.f, 0.f, 1.f }
	} };
}

Matrix Matrix::View(Vector2 eyePosition, float rotation) {
	return Rotation(-rotation) * Translation(-eyePosition.x, -eyePosition.y);
}

Matrix Matrix::ProjectOrthographic(float viewWidth, float viewHeight, float viewRange) {
	return Matrix{ {
		{ 2.f/viewWidth, 0.f, 0.f, 0.f },
		{ 0.f, 2.f/viewHeight, 0.f, 0.f },
		{ 0.f, 0.f, 1.f/viewRange, 0.f },
		{ 0.f, 0.f, 0.f, 1.f }
	} };
}
#pragma endregion